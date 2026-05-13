#!/usr/bin/env bash
# NerdMiner v2 — Flash Script
# Supports: build-then-flash (PlatformIO) or flash pre-built bins (esptool)
set -euo pipefail

# ── Colours ──────────────────────────────────────────────────────────────────
RED='\033[0;31m'; GRN='\033[0;32m'; YEL='\033[1;33m'
CYN='\033[0;36m'; BLD='\033[1m'; RST='\033[0m'

# ── Helpers ───────────────────────────────────────────────────────────────────
die()  { echo -e "${RED}Error: $*${RST}" >&2; exit 1; }
info() { echo -e "${CYN}$*${RST}"; }
ok()   { echo -e "${GRN}$*${RST}"; }
hdr()  { echo -e "\n${BLD}${YEL}$*${RST}"; }

# ── Device table ─────────────────────────────────────────────────────────────
# Format: "Display Name|pio_env|chip|baud"
DEVICES=(
  "NerdMiner V2  (LilyGo T-Display S3)       |NerdminerV2|esp32s3|921600"
  "LilyGo T-HMI                              |NerdminerV2-T-HMI|esp32s3|921600"
  "LilyGo T-Embed                            |Lilygo-T-Embed|esp32s3|921600"
  "LilyGo S3 AMOLED                          |NerdminerV2-S3-AMOLED|esp32s3|921600"
  "LilyGo S3 AMOLED Touch                    |NerdminerV2-S3-AMOLED-TOUCH|esp32s3|921600"
  "LilyGo T-QT Pro                           |NerdminerV2-T-QT|esp32s3|115200"
  "LilyGo S3 Dongle                          |NerdminerV2-S3-DONGLE|esp32s3|921600"
  "LilyGo T-Display V1  (classic ESP32)      |TTGO-T-Display|esp32|921600"
  "ESP32 DevKit V1  (classic)                |ESP32-devKitv1|esp32|921600"
  "ESP32-S3 DevKit C1                        |ESP32-S3-devKitv1|esp32s3|921600"
  "ESP32-S3 DevKit C1 N32R8                  |esp32-s3-devkitc1-n32r8|esp32s3|921600"
  "ESP32-C3 Super Mini                       |ESP32-C3-super-mini|esp32c3|115200"
  "ESP32-C3 DevKit M1                        |ESP32-C3-devKitmv1|esp32c3|115200"
  "ESP32-C3 0.42\" OLED                       |ESP32-C3-042-OLED|esp32c3|115200"
  "ESP32-C3 Spotpear                         |ESP32-C3-spotpear|esp32c3|115200"
  "ESP32-S3 0.42\" OLED                       |ESP32-S3-042-OLED|esp32s3|115200"
  "ESP32-2432S028R  (CYD / Cheap Yellow)     |ESP32-2432S028R|esp32|921600"
  "ESP32-2432S028  (2 USB variant)           |ESP32_2432S028_2USB|esp32|921600"
  "ESP32-2432S024                            |ESP32-2432S024|esp32|921600"
  "ESP32-CAM                                 |esp32cam|esp32|921600"
  "ESP32-D0WD-V3  (WeAct)                    |ESP32-D0WD-V3-weact|esp32|921600"
  "ESP32-S3 Mini  (Wemos/Lolin)              |ESP32-S3-mini-wemos|esp32s3|921600"
  "ESP32-S3 Mini  (WeAct)                    |ESP32-S3-mini-weact|esp32s3|921600"
  "ESP32-S2 Mini  (Wemos/Lolin)              |ESP32-S2-mini-wemos|esp32s2|921600"
  "M5Stick-C                                 |M5Stick-C|esp32|921600"
  "M5Stick-C Plus                            |M5Stick-CPlus|esp32|921600"
  "M5Stick-C Plus2                           |M5Stick-C-Plus2|esp32s3|921600"
  "M5Stack Core  (HAN display)               |han_m5stack|esp32|921600"
  "M5 Stamp S3                               |M5-StampS3|esp32s3|115200"
  "WT32-SC01                                 |wt32-sc01|esp32|921600"
  "WT32-SC01 Plus                            |wt32-sc01-plus|esp32s3|921600"
  "Waveshare S3 Geek                         |NerdminerV2-S3-GEEK|esp32s3|921600"
  "NerdMiner DUO  (unit A)                   |NerdminerV2|esp32s3|921600"
)

# Pre-built bin directories (relative to script location)
declare -A PREBUILT_DIRS=(
  ["NerdminerV2"]="bin/bin LYLYGO TDisplay S3 - no pass"
  ["ESP32-devKitv1"]="bin/bin ESP32-devKit- no pass"
)

# ── Chip → bootloader address ─────────────────────────────────────────────────
boot_addr() {
  case "$1" in
    esp32)   echo "0x1000" ;;
    *)       echo "0x0000" ;;   # esp32s3, esp32s2, esp32c3 all use 0x0000
  esac
}

# ── Detect serial ports ───────────────────────────────────────────────────────
detect_ports() {
  local ports=()
  if [[ "$OSTYPE" == "darwin"* ]]; then
    while IFS= read -r p; do ports+=("$p"); done < \
      <(ls /dev/cu.usbserial-* /dev/cu.SLAB_USB* /dev/cu.wchusbserial* \
           /dev/cu.usbmodem* 2>/dev/null || true)
  else
    while IFS= read -r p; do ports+=("$p"); done < \
      <(ls /dev/ttyUSB* /dev/ttyACM* 2>/dev/null || true)
  fi
  printf '%s\n' "${ports[@]:-}"
}

# ── Pick a serial port ────────────────────────────────────────────────────────
pick_port() {
  mapfile -t PORTS < <(detect_ports)

  if [[ ${#PORTS[@]} -eq 0 ]]; then
    echo -e "${YEL}No serial ports detected automatically.${RST}"
    read -rp "Enter port manually (e.g. /dev/ttyUSB0): " PORT
    [[ -z "$PORT" ]] && die "No port specified."
    return
  fi

  if [[ ${#PORTS[@]} -eq 1 ]]; then
    PORT="${PORTS[0]}"
    info "Auto-selected port: $PORT"
    return
  fi

  hdr "Available serial ports:"
  for i in "${!PORTS[@]}"; do
    printf "  %2d) %s\n" $((i+1)) "${PORTS[$i]}"
  done
  printf "   m) Enter manually\n\n"

  read -rp "Select port [1]: " choice
  choice="${choice:-1}"

  if [[ "$choice" == "m" ]]; then
    read -rp "Enter port: " PORT
  elif [[ "$choice" =~ ^[0-9]+$ ]] && (( choice >= 1 && choice <= ${#PORTS[@]} )); then
    PORT="${PORTS[$((choice-1))]}"
  else
    die "Invalid selection."
  fi
  [[ -z "$PORT" ]] && die "No port specified."
}

# ── Flash with esptool ────────────────────────────────────────────────────────
flash_esptool() {
  local chip="$1" baud="$2" port="$3" bindir="$4"
  local boot_a; boot_a=$(boot_addr "$chip")

  # Locate bin files (handle both 0x0000 and 0x1000 naming)
  local bl part ba fw
  bl=$(find "$bindir" -maxdepth 1 -name "*bootloader*" | head -1)
  part=$(find "$bindir" -maxdepth 1 -name "*partition*" | head -1)
  ba=$(find "$bindir" -maxdepth 1 -name "*boot_app0*" | head -1)
  fw=$(find "$bindir" -maxdepth 1 -name "*firmware*" | head -1)

  [[ -z "$bl" || -z "$part" || -z "$ba" || -z "$fw" ]] && \
    die "Could not find all required .bin files in: $bindir"

  info "Bootloader : $bl"
  info "Partitions : $part"
  info "Boot app0  : $ba"
  info "Firmware   : $fw"
  echo

  esptool.py \
    --chip "$chip" \
    --port "$port" \
    --baud "$baud" \
    --before default_reset \
    --after hard_reset \
    write_flash \
    --flash_mode dio \
    --flash_freq 80m \
    --flash_size detect \
    "$boot_a" "$bl" \
    0x8000   "$part" \
    0xe000   "$ba" \
    0x10000  "$fw"
}

# ── Build & flash with PlatformIO ─────────────────────────────────────────────
flash_pio() {
  local env="$1" port="$2"
  local upload_flag=""
  [[ -n "$port" ]] && upload_flag="--upload-port $port"

  # shellcheck disable=SC2086
  pio run -e "$env" --target upload $upload_flag
}

# ── Check dependencies ────────────────────────────────────────────────────────
HAS_PIO=false; HAS_ESPTOOL=false
command -v pio      &>/dev/null && HAS_PIO=true
command -v esptool.py &>/dev/null && HAS_ESPTOOL=false   # prefer esptool.py
python3 -m esptool  &>/dev/null && HAS_ESPTOOL=true
command -v esptool.py &>/dev/null && HAS_ESPTOOL=true

# ── Banner ────────────────────────────────────────────────────────────────────
clear
echo -e "${BLD}${CYN}"
echo "  ███╗   ██╗███████╗██████╗ ██████╗ "
echo "  ████╗  ██║██╔════╝██╔══██╗██╔══██╗"
echo "  ██╔██╗ ██║█████╗  ██████╔╝██║  ██║"
echo "  ██║╚██╗██║██╔══╝  ██╔══██╗██║  ██║"
echo "  ██║ ╚████║███████╗██║  ██║██████╔╝"
echo "  ╚═╝  ╚═══╝╚══════╝╚═╝  ╚═╝╚═════╝ "
echo -e "${YEL}         Miner v2  —  Flash Tool${RST}"
echo

# ── Choose flashing method ────────────────────────────────────────────────────
hdr "Flash method:"
echo "  1) Build from source + flash  (requires PlatformIO)"
echo "  2) Flash pre-built .bin files (requires esptool)"
echo

if   $HAS_PIO && $HAS_ESPTOOL; then
  read -rp "Method [1]: " METHOD; METHOD="${METHOD:-1}"
elif $HAS_PIO; then
  info "PlatformIO found. Using build + flash."
  METHOD=1
elif $HAS_ESPTOOL; then
  info "esptool found. Using pre-built bins."
  METHOD=2
else
  echo -e "${YEL}Neither PlatformIO nor esptool found.${RST}"
  echo
  echo "Install PlatformIO:  pip install platformio"
  echo "Install esptool:     pip install esptool"
  echo
  echo "Or flash via PlatformIO VS Code extension."
  exit 1
fi

# ── Device selection ──────────────────────────────────────────────────────────
hdr "Select your device:"
for i in "${!DEVICES[@]}"; do
  IFS='|' read -r name _ _ _ <<< "${DEVICES[$i]}"
  printf "  %2d) %s\n" $((i+1)) "$name"
done
echo

read -rp "Device number: " DEV_NUM
[[ "$DEV_NUM" =~ ^[0-9]+$ ]] && (( DEV_NUM >= 1 && DEV_NUM <= ${#DEVICES[@]} )) || \
  die "Invalid selection."

IFS='|' read -r DEV_NAME PIO_ENV CHIP BAUD <<< "${DEVICES[$((DEV_NUM-1))]}"
DEV_NAME=$(echo "$DEV_NAME" | xargs)   # trim whitespace

echo
info "Device : $DEV_NAME"
info "Env    : $PIO_ENV"
info "Chip   : $CHIP"
info "Baud   : $BAUD"

# ── Port selection ────────────────────────────────────────────────────────────
hdr "Serial port:"
pick_port
info "Port   : $PORT"

# ── Confirm ───────────────────────────────────────────────────────────────────
echo
echo -e "${YEL}Ready to flash ${BLD}$DEV_NAME${RST}${YEL} on ${BLD}$PORT${RST}"
read -rp "Continue? [Y/n]: " CONFIRM
CONFIRM="${CONFIRM:-Y}"
[[ "$CONFIRM" =~ ^[Yy] ]] || { echo "Aborted."; exit 0; }
echo

# ── Flash ─────────────────────────────────────────────────────────────────────
if [[ "$METHOD" == "1" ]]; then
  # ── PlatformIO build + flash ─────────────────────────────────────────────
  hdr "Building and flashing with PlatformIO..."
  flash_pio "$PIO_ENV" "$PORT"

else
  # ── esptool pre-built bins ───────────────────────────────────────────────
  BINDIR="${PREBUILT_DIRS[$PIO_ENV]:-}"

  if [[ -z "$BINDIR" || ! -d "$BINDIR" ]]; then
    echo -e "${YEL}No pre-built bins found for this device.${RST}"
    echo
    echo "Pre-built bins are only available for:"
    for key in "${!PREBUILT_DIRS[@]}"; do
      printf "  • %s\n" "$key"
    done
    echo
    if $HAS_PIO; then
      read -rp "Build from source with PlatformIO instead? [Y/n]: " USE_PIO
      USE_PIO="${USE_PIO:-Y}"
      if [[ "$USE_PIO" =~ ^[Yy] ]]; then
        hdr "Building and flashing with PlatformIO..."
        flash_pio "$PIO_ENV" "$PORT"
      else
        echo "Aborted."
        exit 0
      fi
    else
      die "Install PlatformIO (pip install platformio) to build from source."
    fi
  else
    hdr "Flashing pre-built bins with esptool..."
    flash_esptool "$CHIP" "$BAUD" "$PORT" "$BINDIR"
  fi
fi

echo
ok "✓ Flash complete!"
echo
echo -e "  Connect to the device hotspot ${BLD}NerdMinerAP${RST} to configure Wi-Fi."
echo -e "  Password: ${BLD}MineYourCoins${RST}"
echo -e "  Once connected to your Wi-Fi, open ${BLD}http://<device-ip>/${RST} for the dashboard."
echo
