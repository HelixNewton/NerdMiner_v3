#pragma once

#ifdef ENABLE_WEBUI

#define WEBUI_PORT 80

// Set -D WEBUI_AUTH_TOKEN=\"yoursecret\" in platformio.ini to require token auth
#ifndef WEBUI_AUTH_TOKEN
#define WEBUI_AUTH_TOKEN ""
#endif

void webui_init();

// Notification hooks — call from mining context on events
void webui_notify_share_accepted();
void webui_notify_share_rejected();
void webui_notify_pool_disconnected();

#endif // ENABLE_WEBUI
