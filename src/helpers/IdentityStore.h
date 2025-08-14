#pragma once

#if defined(ESP32) || defined(RP2040_PLATFORM)
  #include <FS.h>
  #define FILESYSTEM  fs::FS
#elif defined(NRF52_PLATFORM) || defined(STM32_PLATFORM)
  #if defined(QSPIFLASH)
    #include <CustomLFS_QSPIFlash.h>
    #define FILESYSTEM CustomLFS_QSPIFlash
  #elif defined(EXTRAFS)
    #include <CustomLFS.h>
    #define FILESYSTEM CustomLFS
  #else
    #include <Adafruit_LittleFS.h>
    #define FILESYSTEM  Adafruit_LittleFS

    using namespace Adafruit_LittleFS_Namespace;
  #endif
#endif
#include <Identity.h>

class IdentityStore {
  FILESYSTEM* _fs;
  const char* _dir;
public:
  IdentityStore(FILESYSTEM& fs, const char* dir): _fs(&fs), _dir(dir) { }

  void begin() {
     if (_dir && _dir[0] == '/') { _fs->mkdir(_dir); } 
    
    
    }
  bool load(const char *name, mesh::LocalIdentity& id);
  bool load(const char *name, mesh::LocalIdentity& id, char display_name[], int max_name_sz);
  bool save(const char *name, const mesh::LocalIdentity& id);
  bool save(const char *name, const mesh::LocalIdentity& id, const char display_name[]);
};
