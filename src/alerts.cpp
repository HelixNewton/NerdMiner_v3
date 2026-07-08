#include "alerts.h"
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include "mining.h"                       // getMinerSubscribed()
#include "drivers/storage/storage.h"
#ifdef ENABLE_WIREGUARD
#include "wgManager.h"
#endif

extern TSettings Settings;
extern volatile uint32_t valids;          // block solutions found
extern double best_diff;                  // best share difficulty seen

// One send at a time: the event task and the (synchronous) test button share
// alertSendTo(), and two concurrent TLS handshakes would strain the heap.
static SemaphoreHandle_t alertMx = NULL;

// "NerdMiner-XXXX" from the last two MAC bytes — matches the rest of the UI.
static String deviceName() {
    uint8_t mac[6]; WiFi.macAddress(mac);
    char hn[24];
    snprintf(hn, sizeof(hn), "NerdMiner-%02X%02X", mac[4], mac[5]);
    return String(hn);
}

// Escape a string for embedding in a JSON string literal.
static String jsonEsc(const String& s) {
    String o; o.reserve(s.length() + 8);
    for (size_t i = 0; i < s.length(); i++) {
        char c = s[i];
        switch (c) {
            case '\"': o += "\\\""; break;
            case '\\': o += "\\\\"; break;
            case '\n': o += "\\n";  break;
            case '\r': o += "\\r";  break;
            case '\t': o += "\\t";  break;
            default:
                if ((uint8_t)c < 0x20) { char b[8]; snprintf(b, sizeof(b), "\\u%04x", c); o += b; }
                else o += c;
        }
    }
    return o;
}

bool alertSendTo(const String& url, const String& service, const String& msg) {
    String u = url.length() ? url : Settings.alertUrl;
    String svc = service.length() ? service : Settings.alertService;
    if (u.length() == 0) return false;
    if (WiFi.status() != WL_CONNECTED) return false;

    // Cap the wait: a manual test must not stack up to 8 s behind an in-flight
    // event send. If the task is already sending, bail rather than freeze.
    if (alertMx && xSemaphoreTake(alertMx, pdMS_TO_TICKS(500)) != pdTRUE) {
        Serial.println("[ALERT] busy — another send in progress");
        return false;
    }

    bool https = u.startsWith("https://");
    // A TLS session needs a large transient heap allocation; skip rather than
    // risk an out-of-memory fault inside mbedTLS when the miner is heap-starved.
    if (https && ESP.getFreeHeap() < 45000) {
        Serial.println("[ALERT] skipped — low heap for TLS");
        if (alertMx) xSemaphoreGive(alertMx);
        return false;
    }

    // Client objects must outlive HTTPClient: declare them first so they are
    // destroyed LAST (after ~HTTPClient), even on the began==false path.
    WiFiClientSecure secure;
    WiFiClient plain;
    HTTPClient http;
    bool began;
    if (https) {
        secure.setInsecure();             // no cert pinning — webhook content is low-sensitivity
        began = http.begin(secure, u);
    } else {
        began = http.begin(plain, u);
    }

    bool ok = false;
    if (began) {
        http.setTimeout(6000);
        String body, ctype;
        if (svc == "ntfy") {
            body = msg;                    // ntfy takes the message as the raw body
            ctype = "text/plain";
        } else if (svc == "discord") {
            body = "{\"content\":\"" + jsonEsc(msg) + "\"}";
            ctype = "application/json";
        } else {                           // generic: covers Slack/Mattermost/etc.
            body = "{\"text\":\"" + jsonEsc(msg) + "\",\"content\":\"" + jsonEsc(msg) + "\"}";
            ctype = "application/json";
        }
        http.addHeader("Content-Type", ctype);
        int code = http.POST(body);
        ok = (code >= 200 && code < 300);
        Serial.printf("[ALERT] POST %s -> %d\n", u.c_str(), code);
        http.end();
    } else {
        Serial.println("[ALERT] http.begin failed");
    }

    if (alertMx) xSemaphoreGive(alertMx);
    return ok;
}

bool alertSend(const String& msg) {
    return alertSendTo("", "", msg);
}

static void alert_task(void* pv) {
    (void)pv;
    // Baseline current counters so we never fire for stats restored from NVS.
    uint32_t lastValids = valids;
    bool lastWifi = false, lastPool = false, poolInit = false, sentOnline = false;
#ifdef ENABLE_WIREGUARD
    bool lastVpn = false, vpnInit = false;
#endif

    for (;;) {
        bool wifi = (WiFi.status() == WL_CONNECTED);
        bool configured = (Settings.alertUrl.length() > 0);

        if (wifi && configured) {
            // Coming online (first connect after boot / after a drop).
            if (!sentOnline || !lastWifi) {
                alertSend(deviceName() + " is online");
                sentOnline = true;
            }
            // Block found — the headline event.
            uint32_t v = valids;
            if (v > lastValids) {
                char d[32]; snprintf(d, sizeof(d), "%.6f", best_diff);
                alertSend("\xF0\x9F\x8E\x89 BLOCK FOUND on " + deviceName() +
                          "! total=" + String(v) + ", best diff " + String(d));
            }
            lastValids = v;
            // Pool subscription up/down — suppress the first sample so a
            // healthy boot doesn't fire a bogus "pool connected".
            bool pool = getMinerSubscribed();
            if (!poolInit || pool != lastPool) {
                if (poolInit)
                    alertSend(deviceName() + (pool ? " pool connected" : " pool DISCONNECTED"));
                lastPool = pool; poolInit = true;
            }
#ifdef ENABLE_WIREGUARD
            if (wgIsEnabled()) {
                bool vpn = wgIsActive();
                if (!vpnInit || vpn != lastVpn) {
                    if (vpnInit)  // don't announce the very first sample
                        alertSend(deviceName() + (vpn ? " VPN tunnel up" : " VPN tunnel DOWN"));
                    lastVpn = vpn; vpnInit = true;
                }
            }
#endif
        }
        lastWifi = wifi;
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}

void alerts_init() {
    if (alertMx) return;                   // already started
    alertMx = xSemaphoreCreateMutex();
    // 16 KB stack: the mbedTLS handshake call chain is stack-heavy even though
    // its rx/tx buffers live on the heap — match the WebUI task's headroom.
    xTaskCreate(alert_task, "Alerts", 16384, NULL, 1, NULL);
    Serial.println("[ALERT] task started");
}
