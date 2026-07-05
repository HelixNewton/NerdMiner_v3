#ifndef _NVMEMORY_H_
#define _NVMEMORY_H_

// we only have one implementation right now and nothing to choose from.
#define NVMEM_SPIFFS

#include "../devices/device.h"
#include "storage.h"

// Handles load and store of user settings, except wifi credentials. Those are managed by the wifimanager.
class nvMemory
{
public: 
    nvMemory();
    ~nvMemory();
    bool saveConfig(TSettings* Settings);
    bool loadConfig(TSettings* Settings);
    bool deleteConfig();
    // Fleet (multi-miner dashboard) host list — stored as a raw JSON array string,
    // pre-validated by the caller. Persists across browsers since it lives on-device.
    bool saveFleetHosts(const String& json);
    String loadFleetHosts();
    bool deleteFleetHosts();
private:
    bool init();
    bool Initialized_;
};

#ifndef NVMEM_SPIFFS
#error We need some kind of permanent storage implementation!
#endif //NVMEM_TYPE

#endif // _NVMEMORY_H_
