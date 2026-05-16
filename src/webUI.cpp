#ifdef ENABLE_WEBUI

#include "webUI.h"
#include "webDashboard.h"
#include <Arduino.h>
#include <WebServer.h>
#include <Update.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include "mining.h"
#include "monitor.h"
#include "drivers/storage/storage.h"
#include "drivers/storage/nvMemory.h"
#include "version.h"
#include "timeconst.h"

// ── Extern mining globals ────────────────────────────────────────────────
extern uint32_t templates;
extern uint32_t hashes;
extern uint32_t Mhashes;
extern uint32_t elapsedKHs;
extern uint64_t upTime;
extern volatile uint32_t shares;
extern volatile uint32_t valids;
extern double best_diff;
extern monitor_data mMonitor;
extern TSettings Settings;

// ── Extern function declared in mining.h ────────────────────────────────
// getMinerSubscribed() is defined in mining.cpp

// ── Internal state ───────────────────────────────────────────────────────
static WebServer httpServer(WEBUI_PORT);
static nvMemory nvMem;

// Event flags (set by notify hooks, cleared after status read)
static volatile bool ev_share_accepted  = false;
static volatile bool ev_share_rejected  = false;
static volatile bool ev_pool_disconnect = false;

// Rate-limit config POST: one change per 5 seconds
static unsigned long last_config_save_ms = 0;
#define CONFIG_SAVE_COOLDOWN_MS 5000

// ── Auth helper ───────────────────────────────────────────────────────────
static bool checkAuth() {
#ifdef WEBUI_AUTH_TOKEN
    const String token = String(WEBUI_AUTH_TOKEN);
    if (!token.isEmpty()) {
        if (httpServer.hasHeader("X-API-Token") &&
            httpServer.header("X-API-Token") == token) return true;
        if (httpServer.hasHeader("Authorization")) {
            String auth = httpServer.header("Authorization");
            if (auth.startsWith("Bearer ") && auth.substring(7) == token) return true;
        }
        httpServer.send(401, "application/json",
            "{\"success\":false,\"error\":\"Unauthorized\"}");
        return false;
    }
#endif
    return true;
}

// ── CORS headers ─────────────────────────────────────────────────────────
static void addCors() {
    httpServer.sendHeader("Access-Control-Allow-Origin",  "*");
    httpServer.sendHeader("Access-Control-Allow-Methods", "GET,POST,OPTIONS");
    httpServer.sendHeader("Access-Control-Allow-Headers",
                          "Content-Type,X-API-Token,Authorization");
}

// ── Uptime formatter ─────────────────────────────────────────────────────
static String fmtUptime(uint64_t s) {
    uint32_t d = (uint32_t)(s / 86400);
    uint32_t h = (uint32_t)((s % 86400) / 3600);
    uint32_t m = (uint32_t)((s % 3600) / 60);
    char buf[32];
    if (d > 0) snprintf(buf, sizeof(buf), "%ud %02uh %02um", d, h, m);
    else if (h > 0) snprintf(buf, sizeof(buf), "%uh %02um", h, m);
    else snprintf(buf, sizeof(buf), "%um %02us", m, (uint32_t)(s % 60));
    return String(buf);
}

// ── Wallet masking ────────────────────────────────────────────────────────
static String maskWallet(const char* w) {
    String s(w);
    if (s.length() <= 12) return s;
    return s.substring(0, 6) + "…" + s.substring(s.length() - 6);
}

// ── /  (dashboard) ────────────────────────────────────────────────────────
static void handleRoot() {
    httpServer.send_P(200, "text/html", DASHBOARD_HTML);
}

// ── GET /api/status ────────────────────────────────────────────────────────
static void handleApiStatus() {
    addCors();

    // Snapshot volatile values once
    uint32_t mh  = Mhashes;
    uint32_t h   = hashes;
    uint32_t elk = elapsedKHs;
    uint64_t ut  = upTime;
    uint32_t sh  = shares;
    uint32_t vl  = valids;
    double   bd  = best_diff;
    uint32_t tp  = templates;
    bool     sub = getMinerSubscribed();
    bool     wok = (WiFi.status() == WL_CONNECTED);

    StaticJsonDocument<640> doc;
    doc["hashrate_khs"]  = elk;
    doc["total_mhashes"] = mh;
    doc["shares"]        = sh;
    doc["valids"]        = vl;

    char diffbuf[32];
    snprintf(diffbuf, sizeof(diffbuf), "%.8f", bd);
    doc["best_diff"]     = serialized(diffbuf);

    doc["uptime"]        = (uint32_t)(ut);
    doc["uptime_str"]    = fmtUptime(ut);
    doc["templates"]     = tp;
    doc["pool_connected"]   = wok && sub;
    doc["pool_subscribed"]  = sub;
    doc["wifi_rssi"]     = wok ? WiFi.RSSI() : -100;
    doc["wifi_ssid"]     = wok ? WiFi.SSID() : String("");
    doc["free_heap"]     = (uint32_t)ESP.getFreeHeap();
    doc["total_heap"]    = (uint32_t)ESP.getHeapSize();
    doc["ip"]            = wok ? WiFi.localIP().toString() : String("0.0.0.0");
    doc["pool_url"]      = Settings.PoolAddress;
    doc["pool_port"]     = Settings.PoolPort;
    doc["wallet"]        = String(Settings.BtcWallet);
    doc["firmware"]      = CURRENT_VERSION;

    // Device hostname: "NerdMiner-" + last 4 hex of MAC
    uint8_t mac[6]; WiFi.macAddress(mac);
    char hn[24];
    snprintf(hn, sizeof(hn), "NerdMiner-%02X%02X", mac[4], mac[5]);
    doc["hostname"] = String(hn);

    // Status string
    if (mMonitor.NerdStatus == NM_waitingConfig)  doc["status"] = "waiting_config";
    else if (mMonitor.NerdStatus == NM_Connecting) doc["status"] = "connecting";
    else                                           doc["status"] = "mining";

    // Event flags — expose and clear
    doc["ev_share_accepted"]  = (bool)ev_share_accepted;
    doc["ev_share_rejected"]  = (bool)ev_share_rejected;
    doc["ev_pool_disconnect"] = (bool)ev_pool_disconnect;
    ev_share_accepted  = false;
    ev_share_rejected  = false;
    ev_pool_disconnect = false;

    String out;
    serializeJson(doc, out);
    httpServer.send(200, "application/json", out);
}

// ── GET /api/system ────────────────────────────────────────────────────────
static void handleApiSystem() {
    addCors();

    StaticJsonDocument<512> doc;
    doc["firmware"]       = CURRENT_VERSION;
    doc["total_heap"]     = (uint32_t)ESP.getHeapSize();
    doc["free_heap"]      = (uint32_t)ESP.getFreeHeap();
    doc["min_free_heap"]  = (uint32_t)ESP.getMinFreeHeap();
    doc["cpu_freq_mhz"]   = (uint32_t)ESP.getCpuFreqMHz();
    doc["flash_size"]     = (uint32_t)ESP.getFlashChipSize();
    doc["sdk_version"]    = String(ESP.getSdkVersion());

    uint8_t mac[6]; WiFi.macAddress(mac);
    char macStr[18];
    snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    doc["mac"] = String(macStr);

    char hn[24];
    snprintf(hn, sizeof(hn), "NerdMiner-%02X%02X", mac[4], mac[5]);
    doc["hostname"] = String(hn);

#if defined(CONFIG_IDF_TARGET_ESP32S3)
    doc["chip_model"] = "ESP32-S3";
#elif defined(CONFIG_IDF_TARGET_ESP32S2)
    doc["chip_model"] = "ESP32-S2";
#elif defined(CONFIG_IDF_TARGET_ESP32C3)
    doc["chip_model"] = "ESP32-C3";
#elif defined(CONFIG_IDF_TARGET_ESP32)
    doc["chip_model"] = "ESP32";
#else
    doc["chip_model"] = "ESP32-xx";
#endif

    String out;
    serializeJson(doc, out);
    httpServer.send(200, "application/json", out);
}

// ── GET /api/config ────────────────────────────────────────────────────────
static void handleApiConfigGet() {
    addCors();

    StaticJsonDocument<256> doc;
    doc["pool_url"]   = Settings.PoolAddress;
    doc["pool_port"]  = Settings.PoolPort;
    doc["wallet"]     = String(Settings.BtcWallet);
    doc["pool_pass"]  = String(Settings.PoolPassword);
    doc["timezone"]   = Settings.Timezone;
    doc["save_stats"] = Settings.saveStats;

    String out;
    serializeJson(doc, out);
    httpServer.send(200, "application/json", out);
}

// ── POST /api/config ───────────────────────────────────────────────────────
static void handleApiConfigPost() {
    addCors();
    if (!checkAuth()) return;

    unsigned long now = millis();
    if (now - last_config_save_ms < CONFIG_SAVE_COOLDOWN_MS && last_config_save_ms != 0) {
        httpServer.send(429, "application/json",
            "{\"success\":false,\"error\":\"Rate limited, wait 5s\"}");
        return;
    }

    String body = httpServer.arg("plain");
    if (body.isEmpty()) {
        httpServer.send(400, "application/json",
            "{\"success\":false,\"error\":\"Empty body\"}");
        return;
    }

    StaticJsonDocument<384> doc;
    DeserializationError err = deserializeJson(doc, body);
    if (err) {
        httpServer.send(400, "application/json",
            "{\"success\":false,\"error\":\"JSON parse error\"}");
        return;
    }

    // Validate required fields
    if (!doc.containsKey("pool_url") || !doc.containsKey("wallet")) {
        httpServer.send(400, "application/json",
            "{\"success\":false,\"error\":\"pool_url and wallet required\"}");
        return;
    }

    String poolUrl = doc["pool_url"].as<String>();
    String wallet  = doc["wallet"].as<String>();
    poolUrl.trim(); wallet.trim();

    if (poolUrl.isEmpty() || wallet.isEmpty()) {
        httpServer.send(400, "application/json",
            "{\"success\":false,\"error\":\"pool_url and wallet cannot be empty\"}");
        return;
    }

    // Basic wallet validation: must be at least 26 chars and not default
    if (wallet.length() < 26 || wallet == DEFAULT_WALLETID) {
        httpServer.send(400, "application/json",
            "{\"success\":false,\"error\":\"Invalid wallet address\"}");
        return;
    }

    // Apply settings
    Settings.PoolAddress  = poolUrl;
    Settings.PoolPort     = doc.containsKey("pool_port")
                              ? (int)doc["pool_port"] : Settings.PoolPort;
    if (doc.containsKey("pool_pass"))
        strncpy(Settings.PoolPassword, doc["pool_pass"].as<const char*>(),
                sizeof(Settings.PoolPassword) - 1);
    strncpy(Settings.BtcWallet, wallet.c_str(), sizeof(Settings.BtcWallet) - 1);
    if (doc.containsKey("timezone"))
        Settings.Timezone = (int)doc["timezone"];
    if (doc.containsKey("save_stats"))
        Settings.saveStats = (bool)doc["save_stats"];

    // Clamp port
    if (Settings.PoolPort < 1 || Settings.PoolPort > 65535)
        Settings.PoolPort = DEFAULT_POOLPORT;

    // Save to SPIFFS
    bool ok = nvMem.saveConfig(&Settings);
    last_config_save_ms = millis();

    if (ok) {
        httpServer.send(200, "application/json", "{\"success\":true}");
        // Restart after brief delay so response is sent
        vTaskDelay(800 / portTICK_PERIOD_MS);
        ESP.restart();
    } else {
        httpServer.send(500, "application/json",
            "{\"success\":false,\"error\":\"Failed to save config\"}");
    }
}

// ── POST /api/restart ──────────────────────────────────────────────────────
static void handleApiRestart() {
    addCors();
    if (!checkAuth()) return;
    httpServer.send(200, "application/json", "{\"success\":true}");
    vTaskDelay(500 / portTICK_PERIOD_MS);
    ESP.restart();
}

// ── POST /api/reset ────────────────────────────────────────────────────────
static void handleApiReset() {
    addCors();
    if (!checkAuth()) return;
    httpServer.send(200, "application/json", "{\"success\":true}");
    // Delegate to the existing reset function declared in wManager.h
    vTaskDelay(500 / portTICK_PERIOD_MS);
    nvMem.deleteConfig();
    resetStat();
    ESP.restart();
}

// ── OPTIONS (CORS preflight) ───────────────────────────────────────────────
static void handleOptions() {
    addCors();
    httpServer.send(204, "text/plain", "");
}

// ── POST /api/ota — upload handler (called per chunk) ─────────────────────
static void handleOtaUpload() {
    HTTPUpload& upload = httpServer.upload();

    if (upload.status == UPLOAD_FILE_START) {
        Serial.printf("[OTA] Filename: %s  size: %u\n",
                      upload.filename.c_str(), upload.totalSize);
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
            Serial.print("[OTA] begin error: ");
            Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
            Serial.print("[OTA] write error: ");
            Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) {
            Serial.printf("[OTA] Success: %u bytes\n", upload.totalSize);
        } else {
            Serial.print("[OTA] end error: ");
            Update.printError(Serial);
        }
    }
}

// ── POST /api/ota — response handler (called after upload finishes) ────────
static void handleOtaComplete() {
    addCors();
    if (Update.hasError()) {
        String err = "[OTA error]";
        httpServer.send(500, "application/json",
            "{\"success\":false,\"error\":\"" + err + "\"}");
    } else {
        httpServer.send(200, "application/json", "{\"success\":true}");
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        ESP.restart();
    }
}

// ── GET /api/pool/test ─────────────────────────────────────────────────────
static void handlePoolTest() {
    addCors();
    WiFiClient testClient;
    bool ok = testClient.connect(Settings.PoolAddress.c_str(), Settings.PoolPort);
    testClient.stop();
    if (ok) {
        httpServer.send(200, "application/json",
            "{\"success\":true,\"message\":\"Pool reachable\"}");
    } else {
        httpServer.send(200, "application/json",
            "{\"success\":false,\"message\":\"Cannot connect to pool\"}");
    }
}

// ── 404 handler ────────────────────────────────────────────────────────────
static void handleNotFound() {
    addCors();
    if (httpServer.method() == HTTP_OPTIONS) { handleOptions(); return; }
    httpServer.send(404, "application/json", "{\"error\":\"Not found\"}");
}

// ── WebUI FreeRTOS task ────────────────────────────────────────────────────
static void webui_task(void* pvParameters) {
    (void)pvParameters;
    Serial.printf("[WEBUI] Task started — heap: %u bytes free\n", esp_get_free_heap_size());

    // Collect headers needed for auth / content-type
    const char* hdrs[] = { "X-API-Token", "Authorization", "Content-Type" };
    httpServer.collectHeaders(hdrs, 3);

    // Route table
    httpServer.on("/",              HTTP_GET,     handleRoot);
    httpServer.on("/api/status",    HTTP_GET,     handleApiStatus);
    httpServer.on("/api/system",    HTTP_GET,     handleApiSystem);
    httpServer.on("/api/config",    HTTP_GET,     handleApiConfigGet);
    httpServer.on("/api/config",    HTTP_POST,    handleApiConfigPost);
    httpServer.on("/api/restart",   HTTP_POST,    handleApiRestart);
    httpServer.on("/api/reset",     HTTP_POST,    handleApiReset);
    httpServer.on("/api/pool/test", HTTP_GET,     handlePoolTest);
    httpServer.on("/api/ota",       HTTP_POST,    handleOtaComplete, handleOtaUpload);

    // Catch-all (also handles CORS preflight)
    httpServer.onNotFound(handleNotFound);

    httpServer.begin(WEBUI_PORT);
    Serial.printf("[WEBUI] Ready — http://%s:%d/ (stack HWM: %u)\n",
        WiFi.localIP().toString().c_str(), WEBUI_PORT,
        uxTaskGetStackHighWaterMark(NULL));

    while (true) {
        httpServer.handleClient();
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

// ── Public API ─────────────────────────────────────────────────────────────

void webui_init() {
    BaseType_t res = xTaskCreatePinnedToCore(
        webui_task, "WebUI",
        16384,   // 16KB: generous stack for HTTP parsing + JSON + large PROGMEM sends
        nullptr,
        3,       // priority 3: above miners (1) but below Stratum (4) and Monitor (5)
        nullptr,
        1        // core 1 (WiFi core)
    );
    if (res != pdPASS) {
        Serial.printf("[WEBUI] ERROR: xTaskCreate failed (free heap: %u)\n", esp_get_free_heap_size());
    }
}

void webui_notify_share_accepted()  { ev_share_accepted  = true; }
void webui_notify_share_rejected()  { ev_share_rejected  = true; }
void webui_notify_pool_disconnected() { ev_pool_disconnect = true; }

#endif // ENABLE_WEBUI
