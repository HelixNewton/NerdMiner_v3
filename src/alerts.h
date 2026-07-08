#ifndef _ALERTS_H_
#define _ALERTS_H_

#include <Arduino.h>

// Webhook alerts: the device POSTs a short message to a Discord / ntfy /
// generic-JSON webhook on notable self-events (block found, pool up/down,
// VPN up/down, coming online). Inert until a URL is configured in Settings.

// Start the background alert task (safe to call once, from webui_init()).
void alerts_init();

// Send one message using the SAVED settings. Blocking (does a TLS POST);
// called from the alert task. Returns true on HTTP 2xx.
bool alertSend(const String& msg);

// Send using an explicit url/service instead of the saved ones — used by the
// "Send test" button so a webhook can be verified before it is saved.
// Empty url/service fall back to the saved values.
bool alertSendTo(const String& url, const String& service, const String& msg);

#endif // _ALERTS_H_
