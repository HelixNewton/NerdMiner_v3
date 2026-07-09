#ifdef ENABLE_WEBUI

#include "webUI.h"
#include "webDashboard.h"
#include <Arduino.h>
#include <WebServer.h>
#include <Update.h>
#include <esp_ota_ops.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <cctype>
#include "mining.h"
#include "monitor.h"
#include "drivers/storage/storage.h"
#include "drivers/storage/nvMemory.h"
#include "wgManager.h"
#include "alerts.h"
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

// ── OTA capability probe ──────────────────────────────────────────────────
// huge_app.csv-style tables have a single app slot of subtype ota_0, and
// esp_ota_get_next_update_partition() then returns the RUNNING partition —
// Update.begin() would erase the executing firmware and brick the board.
// A bare NULL check is not enough; the slot must differ from the running one.
static bool otaCapable() {
    const esp_partition_t* next = esp_ota_get_next_update_partition(NULL);
    return next != NULL && next != esp_ota_get_running_partition();
}

// ── Auth helpers ──────────────────────────────────────────────────────────
// authOk: header check only, sends nothing. Usable from upload callbacks,
// which run before the response phase.
static bool authOk() {
#ifdef WEBUI_AUTH_TOKEN
    const String token = String(WEBUI_AUTH_TOKEN);
    if (!token.isEmpty()) {
        if (httpServer.hasHeader("X-API-Token") &&
            httpServer.header("X-API-Token") == token) return true;
        if (httpServer.hasHeader("Authorization")) {
            String auth = httpServer.header("Authorization");
            if (auth.startsWith("Bearer ") && auth.substring(7) == token) return true;
        }
        return false;
    }
#endif
    return true;
}

static bool checkAuth() {
    if (authOk()) return true;
    httpServer.send(401, "application/json",
        "{\"success\":false,\"error\":\"Unauthorized\"}");
    return false;
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

    StaticJsonDocument<1024> doc;
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
    doc["build"]         = BUILD_VERSION;
    doc["chip"]          = ESP.getChipModel();
    // Chip model is too coarse to gate an OTA push: an S3 DevKit and an S3
    // AMOLED are both "ESP32-S3" but need different images. NM_BOARD_ID is the
    // PlatformIO env name, injected by auto_firmware_version.py.
#ifdef NM_BOARD_ID
    doc["board"]         = NM_BOARD_ID;
#endif
    doc["ota"]           = otaCapable();
#ifdef ENABLE_WIREGUARD
    doc["wg_enabled"]    = wgIsEnabled();
    doc["wg_connected"]  = wgIsActive();   // true only after a real handshake
    doc["wg_state"]      = wgState();      // off | connecting | up | failed
#endif

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
    doc["build"]          = BUILD_VERSION;
    doc["chip"]           = ESP.getChipModel();
    doc["ota"]            = otaCapable();
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
    if (!checkAuth()) return;

    StaticJsonDocument<1024> doc;  // 1024: wallet + WG endpoint/keys/psk flag + alert URL
    doc["pool_url"]   = Settings.PoolAddress;
    doc["pool_port"]  = Settings.PoolPort;
    doc["wallet"]     = String(Settings.BtcWallet);
    doc["pool_pass"]  = String(Settings.PoolPassword);
    doc["timezone"]   = Settings.Timezone;
    doc["save_stats"] = Settings.saveStats;
    doc["alert_url"]     = Settings.alertUrl;
    doc["alert_service"] = Settings.alertService;
#ifdef ENABLE_WIREGUARD
    // WireGuard — public/endpoint fields are returned; the private and
    // preshared keys are write-only and never leave the device, only whether
    // one is set.
    // Guarded so non-WG builds don't advertise a VPN section the dashboard
    // would otherwise render (it keys off wg_enabled being present).
    doc["wg_enabled"]         = Settings.wgEnabled;
    doc["wg_local_ip"]        = Settings.wgLocalIP;
    doc["wg_endpoint"]        = Settings.wgEndpoint;
    doc["wg_port"]            = Settings.wgPort;
    doc["wg_peer_public_key"] = Settings.wgPeerPublicKey;
    doc["wg_has_privkey"]     = (Settings.wgPrivateKey.length() > 0);
    doc["wg_has_psk"]         = (Settings.wgPresharedKey.length() > 0);
#endif

    String out;
    serializeJson(doc, out);
    httpServer.send(200, "application/json", out);
}

// ── GET /api/pools — known-pool registry for the dashboard switcher ─────────
static void handleApiPools() {
    addCors();
    size_t n = 0;
    const PoolDefinition* reg = getPoolRegistry(&n);

    StaticJsonDocument<1024> doc;
    JsonArray arr = doc.to<JsonArray>();
    for (size_t i = 0; i < n; ++i) {
        JsonObject o = arr.createNestedObject();
        o["name"]    = reg[i].name;   // const char* stored by reference (zero copy)
        o["host"]    = reg[i].host;
        o["port"]    = reg[i].port;
        o["has_api"] = (reg[i].apiFormat != POOL_API_NONE);
    }

    String out;
    serializeJson(doc, out);
    httpServer.send(200, "application/json", out);
}

// ── GET/POST /api/fleet — Fleet view host list, persisted on-device so it's ──
// shared across every browser that opens this miner's dashboard (not just the
// browser that added the entries to localStorage).
//
// GET returns the stored JSON array, or literal `null` if the list was never
// initialized (lets clients distinguish "cleared" from "first run").
// POST takes deltas — {"add":["host",...],"remove":["host",...]} — merges them
// into the stored list and returns the authoritative result. Deltas commute,
// so concurrent browsers can't erase each other's additions the way a
// full-list replace would.
#define FLEET_MAX_HOSTS  32
#define FLEET_HOST_MAXLEN 64

// host or host:port — host is alnum/dot/dash, port (if present) is 1-65535
static bool isValidFleetHost(const String& h) {
    if (h.length() == 0 || h.length() > FLEET_HOST_MAXLEN) return false;
    int colon = h.indexOf(':');
    int hostLen = (colon >= 0) ? colon : (int)h.length();
    if (hostLen == 0) return false;
    for (int i = 0; i < hostLen; ++i) {
        char c = h[i];
        if (!(isalnum((unsigned char)c) || c == '.' || c == '-')) return false;
    }
    if (colon >= 0) {
        if (h.indexOf(':', colon + 1) >= 0) return false;
        String port = h.substring(colon + 1);
        if (port.length() == 0 || port.length() > 5) return false;
        for (size_t i = 0; i < port.length(); ++i)
            if (!isdigit((unsigned char)port[i])) return false;
        long p = port.toInt();
        if (p < 1 || p > 65535) return false;
    }
    return true;
}

static void handleApiFleetGet() {
    addCors();
    if (!checkAuth()) return;
    httpServer.send(200, "application/json", nvMem.loadFleetHosts());
}

static void handleApiFleetPost() {
    addCors();
    if (!checkAuth()) return;

    String body = httpServer.arg("plain");
    if (body.isEmpty()) {
        httpServer.send(400, "application/json",
            "{\"success\":false,\"error\":\"Empty body\"}");
        return;
    }

    // Heap-allocated (too big for the task stack). Sized for the worst valid
    // body: 32 adds + 32 removes at 64 chars each — ArduinoJson copies every
    // string into the pool when parsing from a String.
    DynamicJsonDocument doc(6144);
    DeserializationError err = deserializeJson(doc, body);
    if (err || !doc.is<JsonObject>()) {
        httpServer.send(400, "application/json",
            "{\"success\":false,\"error\":\"Expected {\\\"add\\\":[],\\\"remove\\\":[]}\"}");
        return;
    }

    // Load current list. If the stored file exists but fails to parse, bail
    // rather than treating it as empty — applying a delta to an empty base
    // would silently erase the whole persisted fleet.
    String hosts[FLEET_MAX_HOSTS];
    size_t n = 0;
    String curStr = nvMem.loadFleetHosts();
    if (curStr != "null") {
        DynamicJsonDocument cur(4096);
        if (deserializeJson(cur, curStr) != DeserializationError::Ok || !cur.is<JsonArray>()) {
            httpServer.send(500, "application/json",
                "{\"success\":false,\"error\":\"Stored fleet list unreadable\"}");
            return;
        }
        for (JsonVariant v : cur.as<JsonArray>()) {
            if (n >= FLEET_MAX_HOSTS) break;
            if (v.is<const char*>()) hosts[n++] = v.as<String>();
        }
    }

    // Apply removals
    if (doc["remove"].is<JsonArray>()) {
        for (JsonVariant v : doc["remove"].as<JsonArray>()) {
            if (!v.is<const char*>()) continue;
            String h = v.as<String>();
            h.trim();
            for (size_t i = 0; i < n; ++i) {
                if (hosts[i] == h) {
                    for (size_t j = i + 1; j < n; ++j) hosts[j - 1] = hosts[j];
                    --n;
                    break;
                }
            }
        }
    }

    // Apply additions (validated, deduped, capped)
    bool truncated = false;
    if (doc["add"].is<JsonArray>()) {
        for (JsonVariant v : doc["add"].as<JsonArray>()) {
            if (!v.is<const char*>()) continue;
            String h = v.as<String>();
            h.trim();
            if (!isValidFleetHost(h)) continue;
            bool dup = false;
            for (size_t i = 0; i < n; ++i) if (hosts[i] == h) { dup = true; break; }
            if (dup) continue;
            if (n >= FLEET_MAX_HOSTS) { truncated = true; continue; }
            hosts[n++] = h;
        }
    }

    // Hosts are charset-validated (no quotes/backslashes possible), safe to embed
    String out = "[";
    for (size_t i = 0; i < n; ++i) {
        if (i) out += ',';
        out += '"';
        out += hosts[i];
        out += '"';
    }
    out += ']';

    bool ok = (out == curStr) || nvMem.saveFleetHosts(out);
    if (ok) {
        String resp = "{\"success\":true,\"truncated\":";
        resp += truncated ? "true" : "false";
        resp += ",\"hosts\":";
        resp += out;
        resp += '}';
        httpServer.send(200, "application/json", resp);
    } else {
        httpServer.send(500, "application/json",
            "{\"success\":false,\"error\":\"Failed to save fleet list\"}");
    }
}

// ── GET /api/discover — on-device mDNS scan for other NerdMiners ────────────
// Each miner advertises _nerdminer._tcp; this queries the LAN for peers and
// returns them, so the dashboard doesn't have to brute-force sweep the /24.
// MDNS.queryService blocks ~2-3s, which is fine on the webui task.
static void handleApiDiscover() {
    addCors();
    if (!checkAuth()) return;
    int n = MDNS.queryService("nerdminer", "tcp");
    String out = "[";
    int emitted = 0;
    for (int i = 0; i < n && emitted < FLEET_MAX_HOSTS; ++i) {
        IPAddress ip = MDNS.IP(i);
        if (ip == IPAddress((uint32_t)0) || ip == WiFi.localIP()) continue;
        if (emitted) out += ',';
        out += "{\"ip\":\"" + ip.toString() + "\"";
        out += ",\"port\":" + String(MDNS.port(i)) + "}";
        ++emitted;
    }
    out += ']';
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

    StaticJsonDocument<1536> doc;  // 1536: pool+wallet, WG keys/endpoint/psk, alert URL
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

    // Webhook alerts
    if (doc.containsKey("alert_url")) {
        Settings.alertUrl = doc["alert_url"].as<String>();
        Settings.alertUrl.trim();
        if (Settings.alertUrl.length() > 200) Settings.alertUrl = "";  // sanity cap
    }
    if (doc.containsKey("alert_service")) {
        String svc = doc["alert_service"].as<String>();
        if (svc == "discord" || svc == "ntfy" || svc == "json")
            Settings.alertService = svc;
    }

#ifdef ENABLE_WIREGUARD
    // WireGuard settings. The private and preshared keys are write-only: only
    // overwrite them when a non-empty value is supplied, so re-saving other
    // fields with the key boxes left blank keeps the stored keys. Guarded so
    // non-WG builds never persist a key they can't use.
    if (doc.containsKey("wg_enabled"))
        Settings.wgEnabled = (bool)doc["wg_enabled"];
    if (doc.containsKey("wg_local_ip")) {
        Settings.wgLocalIP = doc["wg_local_ip"].as<String>();
        Settings.wgLocalIP.trim();
    }
    if (doc.containsKey("wg_endpoint")) {
        String ep = doc["wg_endpoint"].as<String>();
        ep.trim();
        if (ep.length() > 128) {
            httpServer.send(400, "application/json",
                "{\"success\":false,\"error\":\"VPN endpoint too long\"}");
            return;
        }
        Settings.wgEndpoint = ep;
    }
    if (doc.containsKey("wg_port"))
        Settings.wgPort = (int)doc["wg_port"];
    if (doc.containsKey("wg_peer_public_key")) {
        Settings.wgPeerPublicKey = doc["wg_peer_public_key"].as<String>();
        Settings.wgPeerPublicKey.trim();
    }
    if (doc.containsKey("wg_private_key")) {
        String pk = doc["wg_private_key"].as<String>();
        pk.trim();
        if (pk.length() > 0) Settings.wgPrivateKey = pk;
    }
    if (doc.containsKey("wg_preshared_key")) {
        String psk = doc["wg_preshared_key"].as<String>();
        psk.trim();
        if (psk.length() > 0) {
            // 32 raw bytes → exactly 44 base64 chars. Reject early so a typo
            // surfaces here rather than as a silent handshake failure.
            if (psk.length() != 44) {
                httpServer.send(400, "application/json",
                    "{\"success\":false,\"error\":\"Preshared key must be 44 base64 characters\"}");
                return;
            }
            Settings.wgPresharedKey = psk;
        }
    }
    // Explicit clear — a blank box keeps the stored key, so servers configured
    // without a PresharedKey need a way to remove one that was set by mistake.
    if (doc.containsKey("wg_clear_psk") && (bool)doc["wg_clear_psk"])
        Settings.wgPresharedKey = "";
    if (Settings.wgPort < 1 || Settings.wgPort > 65535)
        Settings.wgPort = DEFAULT_WG_PORT;
#endif

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
    nvMem.deleteFleetHosts();
    resetStat();
    ESP.restart();
}

// ── OPTIONS (CORS preflight) ───────────────────────────────────────────────
static void handleOptions() {
    addCors();
    httpServer.send(204, "text/plain", "");
}

// ── POST /api/ota — upload handler (called per chunk) ─────────────────────
// Auth is decided once at UPLOAD_FILE_START, before a single byte hits
// flash — checkAuth() can't run here because upload callbacks execute
// before the response phase.
static bool otaAuthorized = false;
static bool otaNoSlot     = false;
static bool otaBadImage   = false;

// A merged/factory image (bootloader + partition table + app, flashed at 0x0)
// starts with the SAME 0xE9 magic as a bare app image, so Update.begin() accepts
// it and writes the bootloader into the app slot. esp_image_verify can then pass
// on the embedded bootloader header, otadata gets repointed, and the board boots
// into bootloader code mapped at the app offset — a reset loop needing USB
// recovery. Only a merged image carries a partition-table header at 0x8000, so
// sniff for it and refuse before anything is committed.
static const size_t OTA_PT_OFFSET = 0x8000;
static size_t  otaOffset   = 0;
static uint8_t otaSig[8];
static uint8_t otaSigBytes = 0;

static bool looksLikeMergedImage(const uint8_t* s) {
    if (s[0] != 0xAA || s[1] != 0x50) return false;          // ESP_PARTITION_MAGIC
    if (s[2] != 0x00 && s[2] != 0x01) return false;          // type: app | data
    uint32_t off = (uint32_t)s[4] | ((uint32_t)s[5] << 8)
                 | ((uint32_t)s[6] << 16) | ((uint32_t)s[7] << 24);
    return off >= 0x8000 && (off % 0x1000) == 0;             // plausible entry offset
}

static void handleOtaUpload() {
    HTTPUpload& upload = httpServer.upload();

    if (upload.status == UPLOAD_FILE_START) {
        otaAuthorized = authOk();
        if (!otaAuthorized) {
            Serial.println("[OTA] Unauthorized upload rejected");
            return;
        }
        // Never call Update.begin() on a single-slot table: the "next"
        // partition IS the running one and begin() would erase live code.
        otaNoSlot = !otaCapable();
        if (otaNoSlot) {
            Serial.println("[OTA] Refused: no OTA partition on this table");
            return;
        }
        otaBadImage = false;
        otaOffset   = 0;
        otaSigBytes = 0;
        Serial.printf("[OTA] Filename: %s  size: %u\n",
                      upload.filename.c_str(), upload.totalSize);
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) {
            Serial.print("[OTA] begin error: ");
            Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        if (!otaAuthorized || otaNoSlot || otaBadImage || Update.hasError()) return;

        // Capture the 8 bytes at file offset 0x8000, which may straddle chunks.
        size_t chunkStart = otaOffset;
        size_t chunkEnd   = otaOffset + upload.currentSize;
        if (chunkStart < OTA_PT_OFFSET + sizeof(otaSig) && chunkEnd > OTA_PT_OFFSET) {
            size_t from = chunkStart > OTA_PT_OFFSET ? chunkStart : OTA_PT_OFFSET;
            size_t to   = chunkEnd < OTA_PT_OFFSET + sizeof(otaSig)
                        ? chunkEnd : OTA_PT_OFFSET + sizeof(otaSig);
            for (size_t a = from; a < to; a++) {
                otaSig[a - OTA_PT_OFFSET] = upload.buf[a - chunkStart];
                otaSigBytes++;
            }
            if (otaSigBytes >= sizeof(otaSig) && looksLikeMergedImage(otaSig)) {
                otaBadImage = true;
                Update.abort();
                Serial.println("[OTA] Refused: merged/factory image — upload the bare "
                               "app image (firmware.bin), not *_factory.bin");
                return;
            }
        }
        otaOffset = chunkEnd;

        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
            Serial.print("[OTA] write error: ");
            Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_END) {
        if (!otaAuthorized || otaNoSlot || otaBadImage) return;
        if (Update.end(true)) {
            Serial.printf("[OTA] Success: %u bytes\n", upload.totalSize);
        } else {
            Serial.print("[OTA] end error: ");
            Update.printError(Serial);
        }
    } else if (upload.status == UPLOAD_FILE_ABORTED) {
        if (otaAuthorized && !otaNoSlot && !otaBadImage) {
            Update.abort();   // already aborted on the bad-image path
            Serial.println("[OTA] Upload aborted by client");
        }
    }
}

// ── POST /api/ota — response handler (called after upload finishes) ────────
static void handleOtaComplete() {
    addCors();
    bool uploadRan = otaAuthorized;
    otaAuthorized = false;   // never let the flag leak into the next request
    if (!authOk()) {
        httpServer.send(401, "application/json",
            "{\"success\":false,\"error\":\"Unauthorized\"}");
        return;
    }
    if (!uploadRan) {
        // Authorized request, but no firmware file made it through the
        // upload callback (empty/missing multipart part).
        httpServer.send(400, "application/json",
            "{\"success\":false,\"error\":\"No firmware uploaded\"}");
        return;
    }
    if (otaBadImage) {
        otaBadImage = false;
        httpServer.send(400, "application/json",
            "{\"success\":false,\"error\":\"That is a merged/factory image. Upload the bare app image (firmware.bin), not *_factory.bin\"}");
        return;
    }
    if (otaNoSlot) {
        otaNoSlot = false;
        httpServer.send(500, "application/json",
            "{\"success\":false,\"error\":\"No OTA partition — flash this board over USB with an OTA-capable partition table\"}");
        return;
    }
    if (Update.hasError() || !Update.isFinished()) {
        String err = Update.hasError() ? String(Update.errorString())
                                       : String("Incomplete upload");
        // errorString() values are fixed SDK strings; strip quotes defensively
        err.replace("\"", "'");
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

// ── POST /api/alert/test — fire a test webhook ─────────────────────────────
// Optional body {"url":...,"service":...} lets the dashboard test the values
// currently in the form (before saving); otherwise the saved settings are used.
static void handleAlertTest() {
    addCors();
    if (!checkAuth()) return;
    String url, svc;
    String body = httpServer.arg("plain");
    if (body.length()) {
        StaticJsonDocument<384> doc;
        if (!deserializeJson(doc, body)) {
            url = doc["url"] | "";
            svc = doc["service"] | "";
        }
    }
    if (url.length() == 0 && Settings.alertUrl.length() == 0) {
        httpServer.send(400, "application/json",
            "{\"success\":false,\"error\":\"No webhook URL set\"}");
        return;
    }
    bool ok = alertSendTo(url, svc,
                          "\xE2\x9C\x85 NerdMiner test alert \xE2\x80\x94 notifications are working.");
    httpServer.send(ok ? 200 : 502, "application/json",
        ok ? "{\"success\":true}"
           : "{\"success\":false,\"error\":\"Webhook POST failed — check URL/service\"}");
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
    httpServer.on("/api/pools",     HTTP_GET,     handleApiPools);
    httpServer.on("/api/fleet",     HTTP_GET,     handleApiFleetGet);
    httpServer.on("/api/fleet",     HTTP_POST,    handleApiFleetPost);
    httpServer.on("/api/discover",  HTTP_GET,     handleApiDiscover);
    httpServer.on("/api/restart",   HTTP_POST,    handleApiRestart);
    httpServer.on("/api/reset",     HTTP_POST,    handleApiReset);
    httpServer.on("/api/pool/test", HTTP_GET,     handlePoolTest);
    httpServer.on("/api/alert/test",HTTP_POST,    handleAlertTest);
    httpServer.on("/api/ota",       HTTP_POST,    handleOtaComplete, handleOtaUpload);

    // Catch-all (also handles CORS preflight)
    httpServer.onNotFound(handleNotFound);

    httpServer.begin(WEBUI_PORT);
    Serial.printf("[WEBUI] Ready — http://%s:%d/ (stack HWM: %u)\n",
        WiFi.localIP().toString().c_str(), WEBUI_PORT,
        uxTaskGetStackHighWaterMark(NULL));

    // mDNS starts lazily once WiFi is connected (setup may run before that)
    // and restarts after every reconnect — the responder binds to the netif/IP
    // at begin() time, so a DHCP change would otherwise leave it stale.
    // Announces nerdminer-xxxx.local plus the _nerdminer._tcp service that
    // /api/discover on other miners queries for.
    bool wifiWasConnected = false;

    while (true) {
        bool wifiNow = (WiFi.status() == WL_CONNECTED);
        bool wifiJustUp = (wifiNow && !wifiWasConnected);
        if (wifiJustUp) {
            uint8_t mac[6]; WiFi.macAddress(mac);
            char host[24];
            snprintf(host, sizeof(host), "nerdminer-%02x%02x", mac[4], mac[5]);
            MDNS.end();
            if (MDNS.begin(host)) {
                MDNS.addService("nerdminer", "tcp", WEBUI_PORT);
                MDNS.addService("http", "tcp", WEBUI_PORT);
                Serial.printf("[WEBUI] mDNS responder: http://%s.local/\n", host);
            } else {
                Serial.println("[WEBUI] mDNS start failed");
            }
        }
#ifdef ENABLE_WIREGUARD
        wgManagerTick(wifiNow, wifiJustUp);
#endif
        wifiWasConnected = wifiNow;
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
    alerts_init();   // webhook notifications (inert until a URL is configured)
}

void webui_notify_share_accepted()  { ev_share_accepted  = true; }
void webui_notify_share_rejected()  { ev_share_rejected  = true; }
void webui_notify_pool_disconnected() { ev_pool_disconnect = true; }

#endif // ENABLE_WEBUI
