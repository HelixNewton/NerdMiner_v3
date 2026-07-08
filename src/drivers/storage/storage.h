#ifndef _STORAGE_H_
#define _STORAGE_H_

#include <Arduino.h>

// config files

// default settings
#ifndef HAN
#define DEFAULT_SSID		"NerdMinerAP"
#else
#define DEFAULT_SSID		"HanSoloAP"
#endif
#define DEFAULT_WIFIPW		"MineYourCoins"
#define DEFAULT_POOLURL		"public-pool.io"
#define DEFAULT_POOLPASS	"x"
#define DEFAULT_WALLETID	"yourBtcAddress"
#define DEFAULT_POOLPORT	21496
#define DEFAULT_TIMEZONE	2
#define DEFAULT_SAVESTATS	false
#define DEFAULT_INVERTCOLORS	false
#define DEFAULT_BRIGHTNESS	250
// WireGuard VPN (full-tunnel client) — all empty/disabled by default
#define DEFAULT_WG_ENABLED	false
#define DEFAULT_WG_LOCALIP	""
#define DEFAULT_WG_ENDPOINT	""
#define DEFAULT_WG_PORT		51820
#define DEFAULT_WG_PUBKEY	""
#define DEFAULT_WG_PRIVKEY	""
// Webhook alerts (block found / pool / VPN / online) — off until a URL is set
#define DEFAULT_ALERT_URL	""
#define DEFAULT_ALERT_SVC	"discord"

// JSON config files
#define JSON_CONFIG_FILE	"/config.json"
#define JSON_FLEET_FILE		"/fleet.json"

// JSON config file SD card (for user interaction, readme.md)
#define JSON_KEY_SSID		"SSID"
#define JSON_KEY_PASW		"WifiPW"
#define JSON_KEY_POOLURL	"PoolUrl"
#define JSON_KEY_POOLPASS	"PoolPassword"
#define JSON_KEY_WALLETID	"BtcWallet"
#define JSON_KEY_POOLPORT	"PoolPort"
#define JSON_KEY_TIMEZONE	"Timezone"
#define JSON_KEY_STATS2NV	"SaveStats"
#define JSON_KEY_INVCOLOR	"invertColors"
#define JSON_KEY_BRIGHTNESS	"Brightness"
#define JSON_KEY_WG_ENABLED	"wgEnabled"
#define JSON_KEY_WG_LOCALIP	"wgLocalIP"
#define JSON_KEY_WG_ENDPOINT	"wgEndpoint"
#define JSON_KEY_WG_PORT	"wgPort"
#define JSON_KEY_WG_PUBKEY	"wgPeerPublicKey"
#define JSON_KEY_WG_PRIVKEY	"wgPrivateKey"
#define JSON_KEY_ALERT_URL	"alertUrl"
#define JSON_KEY_ALERT_SVC	"alertService"

// JSON config file SPIFFS (different for backward compatibility with existing devices)
#define JSON_SPIFFS_KEY_POOLURL		"poolString"
#define JSON_SPIFFS_KEY_POOLPORT	"portNumber"
#define JSON_SPIFFS_KEY_POOLPASS	"poolPassword"
#define JSON_SPIFFS_KEY_WALLETID	"btcString"
#define JSON_SPIFFS_KEY_TIMEZONE	"gmtZone"
#define JSON_SPIFFS_KEY_STATS2NV	"saveStatsToNVS"
#define JSON_SPIFFS_KEY_INVCOLOR	"invertColors"
#define JSON_SPIFFS_KEY_BRIGHTNESS	"Brightness"

// settings
struct TSettings
{
	String WifiSSID{ DEFAULT_SSID };
	String WifiPW{ DEFAULT_WIFIPW };
	String PoolAddress{ DEFAULT_POOLURL };
	char BtcWallet[80]{ DEFAULT_WALLETID };
	char PoolPassword[80]{ DEFAULT_POOLPASS };
	int PoolPort{ DEFAULT_POOLPORT };
	int Timezone{ DEFAULT_TIMEZONE };
	bool saveStats{ DEFAULT_SAVESTATS };
	bool invertColors{ DEFAULT_INVERTCOLORS };
	int Brightness{ DEFAULT_BRIGHTNESS };
	// WireGuard VPN — a WireGuard base64 key is 44 chars; IPs/hostnames short
	bool wgEnabled{ DEFAULT_WG_ENABLED };
	String wgLocalIP{ DEFAULT_WG_LOCALIP };
	String wgEndpoint{ DEFAULT_WG_ENDPOINT };
	int wgPort{ DEFAULT_WG_PORT };
	String wgPeerPublicKey{ DEFAULT_WG_PUBKEY };
	String wgPrivateKey{ DEFAULT_WG_PRIVKEY };
	// Webhook alerts
	String alertUrl{ DEFAULT_ALERT_URL };
	String alertService{ DEFAULT_ALERT_SVC };
};

#endif // _STORAGE_H_