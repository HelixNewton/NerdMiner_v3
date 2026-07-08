#include "nvMemory.h"

#ifdef NVMEM_SPIFFS

#include <SPIFFS.h>
#include <FS.h>
#include <ArduinoJson.h>

#include "../devices/device.h"
#include "storage.h"

nvMemory::nvMemory() : Initialized_(false){};

nvMemory::~nvMemory()
{
    if (Initialized_)
        SPIFFS.end();
};

/// @brief Save settings to config file on SPIFFS
/// @param TSettings* Settings to be saved.
/// @return true on success
bool nvMemory::saveConfig(TSettings* Settings)
{
    if (init())
    {
        // Save Config in JSON format
        Serial.println(F("SPIFS: Saving configuration."));

        // Create a JSON document (1280: WireGuard keys/endpoint + alert webhook
        // URL would overflow the old 512-byte pool)
        StaticJsonDocument<1280> json;
        json[JSON_SPIFFS_KEY_POOLURL] = Settings->PoolAddress;
        json[JSON_SPIFFS_KEY_POOLPORT] = Settings->PoolPort;
        json[JSON_SPIFFS_KEY_POOLPASS] = Settings->PoolPassword;
        json[JSON_SPIFFS_KEY_WALLETID] = Settings->BtcWallet;
        json[JSON_SPIFFS_KEY_TIMEZONE] = Settings->Timezone;
        json[JSON_SPIFFS_KEY_STATS2NV] = Settings->saveStats;
        json[JSON_SPIFFS_KEY_INVCOLOR] = Settings->invertColors;
        json[JSON_SPIFFS_KEY_BRIGHTNESS] = Settings->Brightness;
        json[JSON_KEY_WG_ENABLED] = Settings->wgEnabled;
        json[JSON_KEY_WG_LOCALIP] = Settings->wgLocalIP;
        json[JSON_KEY_WG_ENDPOINT] = Settings->wgEndpoint;
        json[JSON_KEY_WG_PORT] = Settings->wgPort;
        json[JSON_KEY_WG_PUBKEY] = Settings->wgPeerPublicKey;
        json[JSON_KEY_WG_PRIVKEY] = Settings->wgPrivateKey;
        json[JSON_KEY_ALERT_URL] = Settings->alertUrl;
        json[JSON_KEY_ALERT_SVC] = Settings->alertService;

        // Open config file
        File configFile = SPIFFS.open(JSON_CONFIG_FILE, "w");
        if (!configFile)
        {
            // Error, file did not open
            Serial.println("SPIFS: Failed to open config file for writing");
            return false;
        }

        // Non-secret confirmation only — never dump the raw config to serial:
        // it holds the pool password and the WireGuard private key.
        Serial.printf("SPIFS: pool=%s:%d wallet=%.10s… wg=%s\n",
                      Settings->PoolAddress.c_str(), Settings->PoolPort,
                      Settings->BtcWallet, Settings->wgEnabled ? "on" : "off");
        // Serialize JSON data to write to file
        if (serializeJson(json, configFile) == 0)
        {
            // Error writing file
            Serial.println(F("SPIFS: Failed to write to file"));
            return false;
        }
        // Close file
        configFile.close();
        return true;
    };
    return false;
}

/// @brief Load settings from config file located in SPIFFS.
/// @param TSettings* Struct to update with new settings.
/// @return true on success
bool nvMemory::loadConfig(TSettings* Settings)
{
    // Uncomment if we need to format filesystem
    // SPIFFS.format();

    // Load existing configuration file
    // Read configuration from FS json

    if (init())
    {
        if (SPIFFS.exists(JSON_CONFIG_FILE))
        {
            // The file exists, reading and loading
            File configFile = SPIFFS.open(JSON_CONFIG_FILE, "r");
            if (configFile)
            {
                Serial.println("SPIFS: Loading config file");
                StaticJsonDocument<1280> json;
                DeserializationError error = deserializeJson(json, configFile);
                configFile.close();
                // Do not dump the raw config to serial — it contains the pool
                // password and the WireGuard private key.
                if (!error)
                {
                    Settings->PoolAddress = json[JSON_SPIFFS_KEY_POOLURL] | Settings->PoolAddress;
                    strcpy(Settings->PoolPassword, json[JSON_SPIFFS_KEY_POOLPASS] | Settings->PoolPassword);
                    strcpy(Settings->BtcWallet, json[JSON_SPIFFS_KEY_WALLETID] | Settings->BtcWallet);
                    if (json.containsKey(JSON_SPIFFS_KEY_POOLPORT))
                        Settings->PoolPort = json[JSON_SPIFFS_KEY_POOLPORT].as<int>();
                    if (json.containsKey(JSON_SPIFFS_KEY_TIMEZONE))
                        Settings->Timezone = json[JSON_SPIFFS_KEY_TIMEZONE].as<int>();
                    if (json.containsKey(JSON_SPIFFS_KEY_STATS2NV))
                        Settings->saveStats = json[JSON_SPIFFS_KEY_STATS2NV].as<bool>();
                    if (json.containsKey(JSON_SPIFFS_KEY_INVCOLOR)) {
                        Settings->invertColors = json[JSON_SPIFFS_KEY_INVCOLOR].as<bool>();
                    } else {
                        Settings->invertColors = false;
                    }
                    if (json.containsKey(JSON_SPIFFS_KEY_BRIGHTNESS)) {
                        Settings->Brightness = json[JSON_SPIFFS_KEY_BRIGHTNESS].as<int>();
                    } else {
                        Settings->Brightness = 250;
                    }
                    // WireGuard — absent keys leave the struct defaults intact
                    if (json.containsKey(JSON_KEY_WG_ENABLED))
                        Settings->wgEnabled = json[JSON_KEY_WG_ENABLED].as<bool>();
                    Settings->wgLocalIP = json[JSON_KEY_WG_LOCALIP] | Settings->wgLocalIP;
                    Settings->wgEndpoint = json[JSON_KEY_WG_ENDPOINT] | Settings->wgEndpoint;
                    if (json.containsKey(JSON_KEY_WG_PORT))
                        Settings->wgPort = json[JSON_KEY_WG_PORT].as<int>();
                    Settings->wgPeerPublicKey = json[JSON_KEY_WG_PUBKEY] | Settings->wgPeerPublicKey;
                    Settings->wgPrivateKey = json[JSON_KEY_WG_PRIVKEY] | Settings->wgPrivateKey;
                    Settings->alertUrl = json[JSON_KEY_ALERT_URL] | Settings->alertUrl;
                    Settings->alertService = json[JSON_KEY_ALERT_SVC] | Settings->alertService;
                    return true;
                }
                else
                {
                    // Error loading JSON data
                    Serial.println("SPIFS: Error parsing config file!");
                }
            }
            else
            {
                Serial.println("SPIFS: Error opening config file!");
            }
        }
        else
        {
            Serial.println("SPIFS: No config file available!");
        }
    }
    return false;
}

/// @brief Delete config file from SPIFFS
/// @return true on successs
bool nvMemory::deleteConfig()
{
    Serial.println("SPIFS: Erasing config file..");
    return SPIFFS.remove(JSON_CONFIG_FILE); //Borramos fichero
}

/// @brief Save the Fleet view's host list (already validated JSON array) to SPIFFS
/// @return true on success
bool nvMemory::saveFleetHosts(const String& json)
{
    if (!init()) return false;

    File f = SPIFFS.open(JSON_FLEET_FILE, "w");
    if (!f)
    {
        Serial.println("SPIFS: Failed to open fleet file for writing");
        return false;
    }
    // Require all bytes written — a short write (full FS) must not count as success
    bool ok = (f.print(json) == json.length());
    f.close();
    return ok;
}

/// @brief Load the Fleet view's host list from SPIFFS
/// @return raw JSON array string, or "null" if the list was never initialized
///         (lets clients distinguish "user cleared the list" from "first run")
String nvMemory::loadFleetHosts()
{
    if (init() && SPIFFS.exists(JSON_FLEET_FILE))
    {
        File f = SPIFFS.open(JSON_FLEET_FILE, "r");
        if (f)
        {
            String out = f.readString();
            f.close();
            if (out.length() > 0) return out;
        }
    }
    return "null";
}

/// @brief Delete the Fleet host list file from SPIFFS
/// @return true on success
bool nvMemory::deleteFleetHosts()
{
    return SPIFFS.remove(JSON_FLEET_FILE);
}

/// @brief Prepare and mount SPIFFS
/// @return true on success
bool nvMemory::init()
{
    if (!Initialized_)
    {
        Serial.println("SPIFS: Mounting File System...");
        // May need to make it begin(true) first time you are using SPIFFS
        Initialized_ = SPIFFS.begin(false) || SPIFFS.begin(true);
        Initialized_ ? Serial.println("SPIFS: Mounted") : Serial.println("SPIFS: Mounting failed.");
    }
    else
    {
        Serial.println("SPIFS: Already Mounted");
    }
    return Initialized_;
};

#else

nvMemory::nvMemory() {}
nvMemory::~nvMemory() {}
bool nvMemory::saveConfig(TSettings* Settings) { return false; }
bool nvMemory::loadConfig(TSettings* Settings) { return false; }
bool nvMemory::deleteConfig() { return false; }
bool nvMemory::saveFleetHosts(const String& json) { return false; }
String nvMemory::loadFleetHosts() { return "null"; }
bool nvMemory::deleteFleetHosts() { return false; }
bool nvMemory::init() { return false; }


#endif //NVMEM_TYPE