#ifndef VERSION_H
#define VERSION_H

#define CURRENT_VERSION "V1.8.3"

// Exact build identifier from `git describe` (injected by
// auto_firmware_version.py); falls back to the release version when
// building outside a git checkout.
#ifdef AUTO_VERSION
#define BUILD_VERSION AUTO_VERSION
#else
#define BUILD_VERSION CURRENT_VERSION
#endif

#endif // VERSION_H
