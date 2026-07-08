#ifndef _WG_MANAGER_H_
#define _WG_MANAGER_H_

// WireGuard full-tunnel VPN client. All entry points are no-ops unless the
// firmware is built with -D ENABLE_WIREGUARD and the user has filled in a
// complete WireGuard config in Settings.

#ifdef ENABLE_WIREGUARD

// Drive the tunnel state machine. Call periodically from a task that already
// tracks WiFi state (the WebUI task). `wifiJustConnected` must be true on the
// tick where WiFi transitions down→up so a stale netif can be rebuilt.
void wgManagerTick(bool wifiConnected, bool wifiJustConnected);

bool wgIsEnabled();    // user turned it on in config (regardless of link state)
bool wgIsActive();     // wg.begin() succeeded and the tunnel interface is up

#endif // ENABLE_WIREGUARD
#endif // _WG_MANAGER_H_
