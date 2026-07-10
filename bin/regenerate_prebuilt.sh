#!/usr/bin/env bash
# Rebuild the pre-built .bin sets that flash.sh (method 2, esptool) hands to
# users WITHOUT PlatformIO. Run this after ANY firmware change — otherwise
# those users get stale firmware (this is how the pre-built bins once ended up
# predating the web dashboard: devices mined but never opened port 80).
#
#   ./bin/regenerate_prebuilt.sh              # all boards
#   ./bin/regenerate_prebuilt.sh NerdminerV2  # just one env
#
# Output: bin/prebuilt/<env>/{bootloader,partitions,boot_app0,firmware}.bin
# flash.sh finds these by env name and flashes them at the right offsets.
set -euo pipefail
cd "$(dirname "$0")/.."

BOOT_APP0="$HOME/.platformio/packages/framework-arduinoespressif32/tools/partitions/boot_app0.bin"
OUT="bin/prebuilt"

# Boards offered by flash.sh. Keep in sync with its device table.
ENVS=(
  ESP32-2432S024 ESP32_2432S028_2USB ESP32-2432S028R
  ESP32-C3-042-OLED ESP32-C3-devKitmv1 ESP32-C3-spotpear ESP32-C3-super-mini
  esp32cam ESP32-D0WD-V3-weact ESP32-devKitv1 ESP32-S2-mini-wemos
  ESP32-S3-042-OLED esp32-s3-devkitc1-n32r8 ESP32-S3-devKitv1
  ESP32-S3-mini-weact ESP32-S3-mini-wemos han_m5stack Lilygo-T-Embed
  M5-StampS3 M5Stick-C M5Stick-C-Plus2 M5Stick-CPlus
  NerdminerV2 NerdminerV2-S3-AMOLED NerdminerV2-S3-AMOLED-TOUCH
  NerdminerV2-S3-DONGLE NerdminerV2-S3-GEEK NerdminerV2-T-HMI NerdminerV2-T-QT
  TTGO-T-Display wt32-sc01 wt32-sc01-plus
)
# Envs with the web dashboard (ENABLE_WEBUI) — their firmware MUST contain it.
# The rest are display-only miners with no web UI, which is correct for them.
WEBUI=" ESP32-devKitv1 esp32-s3-devkitc1-n32r8 ESP32-S3-devKitv1 NerdminerV2 NerdminerV2-S3-AMOLED NerdminerV2-T-HMI ESP32-2432S028R ESP32_2432S028_2USB ESP32-2432S024 "

ONLY="${1:-}"
ok=0; fail=0; failed=""
for env in "${ENVS[@]}"; do
  [[ -n "$ONLY" && "$ONLY" != "$env" ]] && continue
  printf '==> %-28s ' "$env"
  if ! pio run -e "$env" >"/tmp/pb_${env}.log" 2>&1; then
    echo "BUILD FAILED (/tmp/pb_${env}.log)"; fail=$((fail+1)); failed="$failed $env"; continue
  fi
  d="$OUT/$env"; mkdir -p "$d"
  cp ".pio/build/$env/bootloader.bin" "$d/bootloader.bin"
  cp ".pio/build/$env/partitions.bin" "$d/partitions.bin"
  cp "$BOOT_APP0"                     "$d/boot_app0.bin"
  cp ".pio/build/$env/firmware.bin"   "$d/firmware.bin"
  if [[ "$WEBUI" == *" $env "* ]] && ! grep -aq "api/status" "$d/firmware.bin"; then
    echo "ERROR: dashboard missing"; fail=$((fail+1)); failed="$failed $env"; continue
  fi
  echo "ok ($(stat -c%s "$d/firmware.bin") bytes)"; ok=$((ok+1))
done
echo "----"
echo "Done: $ok ok, $fail failed.${failed:+ Failed:$failed}"
[[ $fail -eq 0 ]]
