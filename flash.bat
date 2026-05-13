@echo off
setlocal enabledelayedexpansion
title NerdMiner v2 — Flash Tool

:: ── Colours (Windows 10+) ────────────────────────────────────────────────────
for /f %%a in ('echo prompt $E ^| cmd') do set ESC=%%a
set CYN=%ESC%[36m
set YEL=%ESC%[33m
set GRN=%ESC%[32m
set RED=%ESC%[31m
set BLD=%ESC%[1m
set RST=%ESC%[0m

:: ── Banner ────────────────────────────────────────────────────────────────────
cls
echo %CYN%%BLD%
echo   NERD MINER v2  ^|  Flash Tool
echo %RST%
echo.

:: ── Check tools ───────────────────────────────────────────────────────────────
set HAS_PIO=0
set HAS_ESPTOOL=0

where pio >nul 2>&1        && set HAS_PIO=1
where esptool.py >nul 2>&1 && set HAS_ESPTOOL=1
python -m esptool --version >nul 2>&1 && set HAS_ESPTOOL=1

if "%HAS_PIO%"=="0" if "%HAS_ESPTOOL%"=="0" (
  echo %RED%Neither PlatformIO nor esptool found.%RST%
  echo.
  echo Install PlatformIO:  pip install platformio
  echo Install esptool:     pip install esptool
  echo.
  echo Or use the PlatformIO VS Code extension.
  pause
  exit /b 1
)

:: ── Flash method ──────────────────────────────────────────────────────────────
echo %YEL%Flash method:%RST%
echo   1^) Build from source + flash  ^(requires PlatformIO^)
echo   2^) Flash pre-built .bin files ^(requires esptool^)
echo.

if "%HAS_PIO%"=="1" if "%HAS_ESPTOOL%"=="1" (
  set /p METHOD="Method [1]: "
  if "!METHOD!"=="" set METHOD=1
) else if "%HAS_PIO%"=="1" (
  echo PlatformIO found. Using build + flash.
  set METHOD=1
) else (
  echo esptool found. Using pre-built bins.
  set METHOD=2
)

:: ── Device list ───────────────────────────────────────────────────────────────
echo.
echo %YEL%Select your device:%RST%
echo    1^) NerdMiner V2  ^(LilyGo T-Display S3^)
echo    2^) LilyGo T-HMI
echo    3^) LilyGo T-Embed
echo    4^) LilyGo S3 AMOLED
echo    5^) LilyGo S3 AMOLED Touch
echo    6^) LilyGo T-QT Pro
echo    7^) LilyGo T-Display V1  ^(classic ESP32^)
echo    8^) ESP32 DevKit V1  ^(classic^)
echo    9^) ESP32-S3 DevKit C1
echo   10^) ESP32-C3 Super Mini
echo   11^) ESP32-C3 DevKit M1
echo   12^) ESP32-2432S028R  ^(CYD / Cheap Yellow^)
echo   13^) ESP32-2432S028  ^(2 USB variant^)
echo   14^) ESP32-CAM
echo   15^) M5Stick-C
echo   16^) M5Stick-C Plus
echo   17^) M5Stick-C Plus2
echo   18^) M5Stack Core
echo   19^) WT32-SC01
echo   20^) Waveshare S3 Geek
echo.
set /p DEV="Device number: "

:: Map device number → pio_env, chip, baud
if "%DEV%"=="1"  ( set ENV=NerdminerV2          & set CHIP=esp32s3 & set BAUD=921600 )
if "%DEV%"=="2"  ( set ENV=NerdminerV2-T-HMI    & set CHIP=esp32s3 & set BAUD=921600 )
if "%DEV%"=="3"  ( set ENV=Lilygo-T-Embed        & set CHIP=esp32s3 & set BAUD=921600 )
if "%DEV%"=="4"  ( set ENV=NerdminerV2-S3-AMOLED & set CHIP=esp32s3 & set BAUD=921600 )
if "%DEV%"=="5"  ( set ENV=NerdminerV2-S3-AMOLED-TOUCH & set CHIP=esp32s3 & set BAUD=921600 )
if "%DEV%"=="6"  ( set ENV=NerdminerV2-T-QT      & set CHIP=esp32s3 & set BAUD=115200 )
if "%DEV%"=="7"  ( set ENV=TTGO-T-Display        & set CHIP=esp32   & set BAUD=921600 )
if "%DEV%"=="8"  ( set ENV=ESP32-devKitv1        & set CHIP=esp32   & set BAUD=921600 )
if "%DEV%"=="9"  ( set ENV=ESP32-S3-devKitv1     & set CHIP=esp32s3 & set BAUD=921600 )
if "%DEV%"=="10" ( set ENV=ESP32-C3-super-mini   & set CHIP=esp32c3 & set BAUD=115200 )
if "%DEV%"=="11" ( set ENV=ESP32-C3-devKitmv1    & set CHIP=esp32c3 & set BAUD=115200 )
if "%DEV%"=="12" ( set ENV=ESP32-2432S028R       & set CHIP=esp32   & set BAUD=921600 )
if "%DEV%"=="13" ( set ENV=ESP32_2432S028_2USB   & set CHIP=esp32   & set BAUD=921600 )
if "%DEV%"=="14" ( set ENV=esp32cam              & set CHIP=esp32   & set BAUD=921600 )
if "%DEV%"=="15" ( set ENV=M5Stick-C             & set CHIP=esp32   & set BAUD=921600 )
if "%DEV%"=="16" ( set ENV=M5Stick-CPlus         & set CHIP=esp32   & set BAUD=921600 )
if "%DEV%"=="17" ( set ENV=M5Stick-C-Plus2       & set CHIP=esp32s3 & set BAUD=921600 )
if "%DEV%"=="18" ( set ENV=han_m5stack           & set CHIP=esp32   & set BAUD=921600 )
if "%DEV%"=="19" ( set ENV=wt32-sc01             & set CHIP=esp32   & set BAUD=921600 )
if "%DEV%"=="20" ( set ENV=NerdminerV2-S3-GEEK   & set CHIP=esp32s3 & set BAUD=921600 )

if not defined ENV (
  echo %RED%Invalid selection.%RST%
  pause & exit /b 1
)

:: ── Serial port ───────────────────────────────────────────────────────────────
echo.
echo %YEL%Available COM ports:%RST%
for /f "tokens=1,2 delims==" %%a in ('wmic path Win32_SerialPort get DeviceID^,Description /format:list 2^>nul') do (
  if "%%a"=="Description" set DESC=%%b
  if "%%a"=="DeviceID"    echo   %%b  ^(!DESC!^)
)
echo.
set /p PORT="COM port (e.g. COM3): "
if "%PORT%"=="" (
  echo %RED%No port entered.%RST%
  pause & exit /b 1
)

:: ── Confirm ───────────────────────────────────────────────────────────────────
echo.
echo %YEL%Ready to flash env=%ENV%  chip=%CHIP%  baud=%BAUD%  port=%PORT%%RST%
set /p CONFIRM="Continue? [Y/n]: "
if /i "%CONFIRM%"=="n" ( echo Aborted. & pause & exit /b 0 )
echo.

:: ── Flash ─────────────────────────────────────────────────────────────────────
if "%METHOD%"=="1" goto :flash_pio
goto :flash_esptool

:flash_pio
echo %CYN%Building and flashing with PlatformIO...%RST%
pio run -e %ENV% --target upload --upload-port %PORT%
goto :done

:flash_esptool
:: Determine bin directory for pre-built devices
set BINDIR=
if "%ENV%"=="NerdminerV2"    set "BINDIR=bin\bin LYLYGO TDisplay S3 - no pass"
if "%ENV%"=="ESP32-devKitv1" set "BINDIR=bin\bin ESP32-devKit- no pass"

if "%BINDIR%"=="" (
  echo %YEL%No pre-built bins for this device.%RST%
  if "%HAS_PIO%"=="1" (
    echo.
    set /p USE_PIO="Build from source with PlatformIO? [Y/n]: "
    if /i not "!USE_PIO!"=="n" goto :flash_pio
  ) else (
    echo Install PlatformIO ^(pip install platformio^) to build from source.
  )
  pause & exit /b 1
)

:: Bootloader address: 0x0000 for S3/S2/C3, 0x1000 for classic ESP32
set BOOT_ADDR=0x0000
if "%CHIP%"=="esp32" set BOOT_ADDR=0x1000

:: Find bin files
for %%f in ("%BINDIR%\*bootloader*") do set BL=%%f
for %%f in ("%BINDIR%\*partition*")  do set PT=%%f
for %%f in ("%BINDIR%\*boot_app0*")  do set BA=%%f
for %%f in ("%BINDIR%\*firmware*")   do set FW=%%f

echo %CYN%Flashing pre-built bins...%RST%
echo   Bootloader : %BL%
echo   Partitions : %PT%
echo   Boot app0  : %BA%
echo   Firmware   : %FW%
echo.

esptool.py --chip %CHIP% --port %PORT% --baud %BAUD% ^
  --before default_reset --after hard_reset ^
  write_flash --flash_mode dio --flash_freq 80m --flash_size detect ^
  %BOOT_ADDR% "%BL%" ^
  0x8000 "%PT%" ^
  0xe000 "%BA%" ^
  0x10000 "%FW%"

:done
echo.
echo %GRN%Flash complete!%RST%
echo.
echo   Connect to the hotspot %BLD%NerdMinerAP%RST% to configure Wi-Fi.
echo   Password: %BLD%MineYourCoins%RST%
echo   Then open http://^<device-ip^)/ for the dashboard.
echo.
pause
