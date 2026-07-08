#!/usr/bin/env bash
# Rebuild the pre-built .bin sets that flash.sh (method 2, esptool) hands to
# users WITHOUT PlatformIO. Run this whenever the firmware changes, otherwise
# those users get stale firmware — this is exactly how the pre-built bins ended
# up predating the web dashboard (device mined but never opened port 80).
#
# Usage:  ./bin/regenerate_prebuilt.sh
set -euo pipefail
cd "$(dirname "$0")/.."

BOOT_APP0="$HOME/.platformio/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin"

# env | pre-built dir | bootloader flash offset (esp32=0x1000, S3=0x0000)
SETS=(
  "ESP32-devKitv1|bin/bin ESP32-devKit- no pass|0x1000"
  "NerdminerV2|bin/bin LYLYGO TDisplay S3 - no pass|0x0000"
)

for entry in "${SETS[@]}"; do
  IFS='|' read -r env dir bootoff <<< "$entry"
  echo "==> Building $env (default flags — no WireGuard; VPN stays opt-in)"
  pio run -e "$env"
  b=".pio/build/$env"
  cp "$b/bootloader.bin" "$dir/${bootoff}_bootloader.bin"
  cp "$b/partitions.bin" "$dir/0x8000_partitions.bin"
  cp "$BOOT_APP0"        "$dir/0xe000_boot_app0.bin"
  cp "$b/firmware.bin"   "$dir/0x10000_firmware.bin"

  # sanity: the dashboard MUST be present, or we shipped a mining-only build
  if ! grep -aq "api/status" "$dir/0x10000_firmware.bin"; then
    echo "ERROR: $env firmware has no web dashboard — aborting" >&2
    exit 1
  fi
  echo "    ok: $dir  ($(stat -c%s "$dir/0x10000_firmware.bin") bytes, dashboard present)"
done
echo "Done. Commit the updated bin/ files."
