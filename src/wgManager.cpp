#ifdef ENABLE_WIREGUARD

#include <Arduino.h>
#include <WiFi.h>
#include <NTPClient.h>
#include <time.h>
#include <sys/time.h>
#include <mbedtls/base64.h>

#include "lwip/err.h"
#include "lwip/ip.h"
#include "lwip/ip_addr.h"
#include "lwip/netif.h"
#include "lwip/netdb.h"

extern "C" {
#include "wireguardif.h"
#include "wireguard-platform.h"
}

#include "wgManager.h"
#include "drivers/storage/storage.h"

extern TSettings Settings;
extern NTPClient timeClient;   // defined in monitor.cpp

// The Arduino WireGuard wrapper is deliberately not used here. Its begin()
// hard-codes preshared_key = NULL, so any server that issues a PresharedKey
// (FRITZ!Box, wg-easy, PiVPN) can never complete a handshake. It also installs
// the tunnel as the default route *before* the handshake, which blackholes all
// off-subnet traffic when the handshake never lands. Driving wireguardif
// directly lets us carry a PSK and only take the default route once the peer
// is genuinely up.

static struct netif  wgNetifStruct;
static struct netif* wgNetif = NULL;
static struct netif* prevDefaultNetif = NULL;
static uint8_t       wgPeerIdx = WIREGUARDIF_INVALID_INDEX;

enum WgPhase : uint8_t { WG_IDLE, WG_HANDSHAKING, WG_UP };
static WgPhase wgPhase = WG_IDLE;

static bool          wgGaveUp = false;       // stop attempting until reconfigure/reboot
static uint8_t       wgAttempts = 0;
static unsigned long wgLastAttempt = 0;
static unsigned long wgHandshakeStarted = 0;

// Epochs before this (2023-11-14) are treated as "clock not set yet".
static const unsigned long WG_EPOCH_FLOOR   = 1700000000UL;
static const unsigned long WG_RETRY_MS      = 15000;
// WireGuard re-sends a handshake initiation every ~5s, so this allows ~4 tries.
static const unsigned long WG_HANDSHAKE_MS  = 20000;
static const uint8_t       WG_MAX_ATTEMPTS  = 3;
// Hold the server's NAT mapping open so it can reach us between shares.
static const uint16_t      WG_KEEPALIVE_SEC = 25;

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

// A WireGuard preshared key is 32 raw bytes carried as 44 base64 chars.
static bool decodePsk(const String& b64, uint8_t out[32]) {
    if (b64.length() == 0) return false;
    size_t olen = 0;
    int rc = mbedtls_base64_decode(out, 32, &olen,
                                   (const unsigned char*)b64.c_str(), b64.length());
    return (rc == 0 && olen == 32);
}

static bool resolveEndpoint(ip_addr_t* out) {
    IPAddress literal;
    if (literal.fromString(Settings.wgEndpoint)) {
        ip_addr_t t = IPADDR4_INIT((uint32_t)literal);
        *out = t;
        return true;
    }
    struct addrinfo hint;
    memset(&hint, 0, sizeof(hint));
    struct addrinfo* res = NULL;
    if (lwip_getaddrinfo(Settings.wgEndpoint.c_str(), NULL, &hint, &res) != 0 || res == NULL)
        return false;
    struct in_addr a4 = ((struct sockaddr_in*)res->ai_addr)->sin_addr;
    ip_addr_t t;
    memset(&t, 0, sizeof(t));
    inet_addr_to_ip4addr(ip_2_ip4(&t), &a4);
    lwip_freeaddrinfo(res);
    *out = t;
    return true;
}

static bool wgPeerUp() {
    return wgNetif
        && wgPeerIdx != WIREGUARDIF_INVALID_INDEX
        && wireguardif_peer_is_up(wgNetif, wgPeerIdx, NULL, NULL) == ERR_OK;
}

static void wgStop() {
    if (!wgNetif) { wgPhase = WG_IDLE; return; }
    // Hand the default route back before tearing the netif down, or lwIP is
    // left pointing at a netif that no longer exists.
    if (netif_default == wgNetif) netif_set_default(prevDefaultNetif);
    prevDefaultNetif = NULL;
    if (wgPeerIdx != WIREGUARDIF_INVALID_INDEX) {
        wireguardif_disconnect(wgNetif, wgPeerIdx);
        wireguardif_remove_peer(wgNetif, wgPeerIdx);
        wgPeerIdx = WIREGUARDIF_INVALID_INDEX;
    }
    wireguardif_shutdown(wgNetif);
    netif_remove(wgNetif);
    wgNetif = NULL;
    wgPhase = WG_IDLE;
}

static void wgStart() {
    IPAddress local;
    if (!local.fromString(Settings.wgLocalIP)) {
        Serial.printf("[WG] Invalid tunnel IP '%s' — fix it in Settings\n",
                      Settings.wgLocalIP.c_str());
        wgGaveUp = true;   // un-retryable: a bad IP can never succeed
        return;
    }

    uint8_t psk[32];
    bool hasPsk = decodePsk(Settings.wgPresharedKey, psk);
    if (Settings.wgPresharedKey.length() && !hasPsk) {
        Serial.println("[WG] Preshared key is not 32 bytes of base64 — fix it in Settings");
        wgGaveUp = true;
        return;
    }

    ip_addr_t endpoint;
    if (!resolveEndpoint(&endpoint)) {
        Serial.printf("[WG] Cannot resolve endpoint '%s' — will retry\n",
                      Settings.wgEndpoint.c_str());
        return;
    }

    ip_addr_t ipaddr  = IPADDR4_INIT((uint32_t)local);
    ip_addr_t netmask = IPADDR4_INIT_BYTES(255, 255, 255, 255);
    ip_addr_t gateway = IPADDR4_INIT_BYTES(0, 0, 0, 0);

    struct wireguardif_init_data init;
    init.private_key = Settings.wgPrivateKey.c_str();
    init.listen_port = (uint16_t)Settings.wgPort;
    init.bind_netif  = NULL;   // wireguardif_init pins sends to the STA netif

    memset(&wgNetifStruct, 0, sizeof(wgNetifStruct));
    wgNetif = netif_add(&wgNetifStruct, ip_2_ip4(&ipaddr), ip_2_ip4(&netmask),
                        ip_2_ip4(&gateway), &init, &wireguardif_init, &ip_input);
    if (!wgNetif) {
        Serial.println("[WG] netif_add failed (bad private key?) — will retry");
        return;
    }
    netif_set_up(wgNetif);

    struct wireguardif_peer peer;
    wireguardif_peer_init(&peer);
    ip_addr_t anyIp   = IPADDR4_INIT_BYTES(0, 0, 0, 0);
    ip_addr_t anyMask = IPADDR4_INIT_BYTES(0, 0, 0, 0);
    peer.public_key    = Settings.wgPeerPublicKey.c_str();
    peer.preshared_key = hasPsk ? psk : NULL;   // NULL == the all-zero PSK
    peer.allowed_ip    = anyIp;                 // 0.0.0.0/0 → full tunnel
    peer.allowed_mask  = anyMask;
    peer.endpoint_ip   = endpoint;
    peer.endport_port  = (uint16_t)Settings.wgPort;
    peer.keep_alive    = WG_KEEPALIVE_SEC;

    wireguard_platform_init();
    if (wireguardif_add_peer(wgNetif, &peer, &wgPeerIdx) != ERR_OK
            || wgPeerIdx == WIREGUARDIF_INVALID_INDEX) {
        Serial.println("[WG] add_peer failed (bad server public key?) — will retry");
        wgStop();
        return;
    }

    // Start the handshake, but leave the default route alone until the peer is
    // actually up — a tunnel that never handshakes must not swallow the LAN.
    wireguardif_connect(wgNetif, wgPeerIdx);
    wgPhase = WG_HANDSHAKING;
    wgHandshakeStarted = millis();
    Serial.printf("[WG] Handshaking with %s:%d as %s (preshared key: %s)…\n",
                  Settings.wgEndpoint.c_str(), Settings.wgPort,
                  Settings.wgLocalIP.c_str(), hasPsk ? "yes" : "no");
}

// Reset the attempt counters — call when the situation that caused a give-up
// may have changed (disabled/reconfigured, or WiFi just reconnected).
static void wgResetAttempts() {
    wgGaveUp = false;
    wgAttempts = 0;
    wgLastAttempt = 0;
}

static void wgCountFailure(unsigned long now) {
    wgLastAttempt = now;
    if (++wgAttempts >= WG_MAX_ATTEMPTS) {
        wgGaveUp = true;
        Serial.println("[WG] Giving up — re-save the VPN settings to retry");
    }
}

void wgManagerTick(bool wifiConnected, bool wifiJustConnected) {
    // Config incomplete / disabled → ensure the tunnel is torn down.
    if (!wgConfigComplete()) { wgStop(); wgResetAttempts(); return; }

    // The tunnel rides on the WiFi link; drop it with the link.
    if (!wifiConnected) { wgStop(); return; }

    // A reconnect rebinds the underlying interface/IP — rebuild the netif and
    // give bring-up a fresh set of attempts.
    if (wifiJustConnected) { wgStop(); wgResetAttempts(); }

    unsigned long now = millis();

    switch (wgPhase) {
    case WG_HANDSHAKING:
        if (wgPeerUp()) {
            prevDefaultNetif = netif_default;
            netif_set_default(wgNetif);
            wgPhase = WG_UP;
            wgAttempts = 0;
            Serial.println("[WG] Handshake OK — tunnel up (full-tunnel default route)");
        } else if (now - wgHandshakeStarted > WG_HANDSHAKE_MS) {
            Serial.println("[WG] No handshake response. Check: server public key, "
                           "preshared key, endpoint port, and that this device's "
                           "public key is a registered peer on the server.");
            wgStop();
            wgCountFailure(now);
        }
        return;

    case WG_UP:
        // Handshake expired and could not be renewed: hand the default route
        // back so the miner keeps NTP/alerts/dashboard instead of going dark.
        if (!wgPeerUp()) {
            Serial.println("[WG] Tunnel lost — restoring direct route, will retry");
            wgStop();
            wgLastAttempt = now;
        }
        return;

    case WG_IDLE:
    default:
        break;
    }

    if (wgGaveUp) return;
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
    wgStart();
    if (wgPhase == WG_IDLE && !wgGaveUp) wgCountFailure(now);   // start failed outright
}

bool wgIsEnabled() { return Settings.wgEnabled; }
bool wgIsActive()  { return wgPhase == WG_UP; }

const char* wgState() {
    if (!Settings.wgEnabled) return "off";
    if (wgGaveUp)            return "failed";
    return (wgPhase == WG_UP) ? "up" : "connecting";
}

#endif // ENABLE_WIREGUARD
