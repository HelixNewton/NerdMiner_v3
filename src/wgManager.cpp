#ifdef ENABLE_WIREGUARD

#include <Arduino.h>
#include <WiFi.h>
#include <WireGuard-ESP32.h>
#include <NTPClient.h>
#include <time.h>
#include <sys/time.h>
#include "wgManager.h"
#include "drivers/storage/storage.h"

extern TSettings Settings;
extern NTPClient timeClient;   // defined in monitor.cpp

static WireGuard wg;
static bool wgStarted = false;
static bool wgGaveUp  = false;          // stop attempting until reconfigure/reboot
static uint8_t wgAttempts = 0;
static unsigned long wgLastAttempt = 0;

// Epochs before this (2023-11-14) are treated as "clock not set yet".
static const unsigned long WG_EPOCH_FLOOR = 1700000000UL;
// wg.begin() blocks up to ~10s on DNS failure inside this (WebUI) task, so
// retries must be throttled and bounded — a misconfigured tunnel must not
// freeze the dashboard on every tick.
static const unsigned long WG_RETRY_MS   = 15000;
static const uint8_t       WG_MAX_ATTEMPTS = 3;

static bool wgConfigComplete() {
    return Settings.wgEnabled
        && Settings.wgLocalIP.length()
        && Settings.wgEndpoint.length()
        && Settings.wgPeerPublicKey.length()
        && Settings.wgPrivateKey.length();
}

// The handshake stamps a TAI64N timestamp from the system clock. This firmware
// keeps time only in NTPClient, so seed the system clock from it once — else
// every reboot sends a near-1970 timestamp and the server's replay window can
// reject the reconnect.
static bool ensureSystemClock() {
    if (time(nullptr) > (time_t)WG_EPOCH_FLOOR) return true;
    // timeClient carries the user's timezone offset (setTimeOffset in
    // monitor.cpp); strip it so the system clock is UTC. Setting local time
    // here would make a later timezone change move the clock backwards and
    // trip the WG server's per-peer replay window.
    long epoch = (long)timeClient.getEpochTime() - 3600L * Settings.Timezone;
    if (epoch < (long)WG_EPOCH_FLOOR) return false;   // NTP not ready — retry next tick
    struct timeval tv;
    tv.tv_sec  = (time_t)epoch;
    tv.tv_usec = 0;
    settimeofday(&tv, nullptr);
    return true;
}

static void wgStart() {
    IPAddress local;
    if (!local.fromString(Settings.wgLocalIP)) {
        Serial.printf("[WG] Invalid tunnel IP '%s' — fix it in Settings\n",
                      Settings.wgLocalIP.c_str());
        wgGaveUp = true;   // un-retryable: a bad IP can never succeed
        return;
    }
    Serial.printf("[WG] Bringing up tunnel %s via %s:%d\n",
                  Settings.wgLocalIP.c_str(),
                  Settings.wgEndpoint.c_str(), Settings.wgPort);
    // The single-peer begin() overload installs the WG netif as the default
    // route with allowed-ips 0.0.0.0/0 → full tunnel. LAN/dashboard/fleet
    // traffic stays direct (same-subnet route); only off-subnet traffic
    // (the pool) is tunneled.
    bool ok = wg.begin(local,
                       Settings.wgPrivateKey.c_str(),
                       Settings.wgEndpoint.c_str(),
                       Settings.wgPeerPublicKey.c_str(),
                       (uint16_t)Settings.wgPort);
    wgStarted = ok;
    Serial.println(ok ? "[WG] Tunnel interface up (full-tunnel default route)"
                      : "[WG] begin() failed (DNS/endpoint/key) — will retry");
}

static void wgStop() {
    if (!wgStarted) return;
    wg.end();
    wgStarted = false;
    Serial.println("[WG] Tunnel down");
}

// Reset the attempt counters — call when the situation that caused a give-up
// may have changed (disabled/reconfigured, or WiFi just reconnected).
static void wgResetAttempts() {
    wgGaveUp = false;
    wgAttempts = 0;
    wgLastAttempt = 0;
}

void wgManagerTick(bool wifiConnected, bool wifiJustConnected) {
    // Config incomplete / disabled → ensure the tunnel is torn down.
    if (!wgConfigComplete()) { wgStop(); wgResetAttempts(); return; }

    // The tunnel rides on the WiFi link; drop it with the link.
    if (!wifiConnected) { wgStop(); return; }

    // A reconnect rebinds the underlying interface/IP — rebuild the netif and
    // give bring-up a fresh set of attempts.
    if (wifiJustConnected) { wgStop(); wgResetAttempts(); }

    if (wgStarted || wgGaveUp) return;

    // Throttle: wg.begin() can block ~10s on DNS, so never re-attempt more
    // than once per WG_RETRY_MS, and give up after WG_MAX_ATTEMPTS so a
    // misconfigured tunnel can't stutter the dashboard indefinitely.
    unsigned long now = millis();
    if (wgLastAttempt != 0 && (now - wgLastAttempt) < WG_RETRY_MS) return;

    if (!ensureSystemClock()) {
        static bool loggedWait = false;
        if (!loggedWait) {
            Serial.println("[WG] Waiting for NTP time before starting tunnel…");
            loggedWait = true;
        }
        return;   // cheap check, no blocking — keep polling until NTP is ready
    }

    wgLastAttempt = now;
    wgAttempts++;
    wgStart();
    if (!wgStarted && !wgGaveUp && wgAttempts >= WG_MAX_ATTEMPTS) {
        wgGaveUp = true;
        Serial.println("[WG] Giving up after repeated failures — "
                       "check keys/endpoint and re-save to retry");
    }
}

bool wgIsEnabled() { return Settings.wgEnabled; }
bool wgIsActive()  { return wgStarted; }

#endif // ENABLE_WIREGUARD
