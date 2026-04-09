#!/bin/bash
set -e

# =============================================================================
# Mondrian Run Script
# Installs APK, launches the app, and monitors progress until completion
# =============================================================================

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
APK_PATH="$SCRIPT_DIR/android/Mondrian/build/outputs/apk/debug/Mondrian-debug.apk"
PKG="hcs.offloading.mondrian"
PROGRESS_DIR="log"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Read log_dir from config to find the progress file
LOG_DIR=$(python3 -c "
import json, sys
try:
    cfg = json.load(open('$SCRIPT_DIR/android/config.json'))
    # e.g. /data/data/hcs.offloading.mondrian/log/240101000000
    print(cfg['log_dir'].split('$PKG/')[-1])
except: sys.exit(1)
" 2>/dev/null) || LOG_DIR="log/240101000000"

PROGRESS_FILE="$LOG_DIR/progress"

# -------------------------------------------------------------------------
# Pre-flight checks
# -------------------------------------------------------------------------
if [ ! -f "$APK_PATH" ]; then
    echo -e "${RED}ERROR: APK not found.${NC}"
    echo "  Run ./setup.sh first."
    exit 1
fi

if ! adb devices | grep -q "device$"; then
    echo -e "${RED}ERROR: No device connected.${NC}"
    echo "  Connect a device with USB debugging enabled, then retry."
    exit 1
fi

# Auto-select device when multiple are connected
if [ -z "$ANDROID_SERIAL" ]; then
    DEVICES=($(adb devices | grep "device$" | awk '{print $1}'))
    if [ ${#DEVICES[@]} -gt 1 ]; then
        echo "Multiple devices detected:"
        for i in "${!DEVICES[@]}"; do
            echo "  [$i] ${DEVICES[$i]}"
        done
        read -rp "Select device [0]: " choice
        choice=${choice:-0}
        export ANDROID_SERIAL="${DEVICES[$choice]}"
        echo "Using: $ANDROID_SERIAL"
    fi
fi

# -------------------------------------------------------------------------
# Install & Launch
# -------------------------------------------------------------------------
echo "Installing APK..."
adb install -r "$APK_PATH" > /dev/null
echo -e "${GREEN}APK installed.${NC}"

adb shell am force-stop "$PKG" 2>/dev/null || true

echo "Launching Mondrian..."
adb shell am start -n "$PKG/.MainActivity" > /dev/null
echo ""

# -------------------------------------------------------------------------
# Monitor progress
# -------------------------------------------------------------------------
cleanup() {
    printf "\n"
    echo -e "${YELLOW}Monitoring stopped. The app may still be running on the device.${NC}"
    echo "  Stop:   adb shell am force-stop $PKG"
    echo "  Logs:   ./pull.sh"
    exit 0
}
trap cleanup INT

BAR_WIDTH=40

while true; do
    sleep 2

    # Check if the app is still alive
    if ! adb shell pidof "$PKG" > /dev/null 2>&1; then
        # App exited — check if it completed or crashed
        PROGRESS=$(adb shell run-as "$PKG" cat "$PROGRESS_FILE" 2>/dev/null || echo "")
        if [ "$PROGRESS" = "1.000" ]; then
            break
        fi
        # Check for crash
        ABORT=$(adb logcat -d -s DEBUG:F | grep -c "Abort message" 2>/dev/null || echo "0")
        if [ "$ABORT" -gt 0 ]; then
            printf "\r\033[K"
            ABORT_MSG=$(adb logcat -d | grep "Abort message" | tail -1 | sed 's/.*Abort message: //')
            echo -e "${RED}App crashed.${NC}"
            echo "  $ABORT_MSG"
            echo ""
            echo "  Full crash log: adb logcat -d | grep -A20 'DEBUG.*signal'"
            exit 1
        fi
        printf "\r\033[K"
        echo -e "${RED}App exited unexpectedly.${NC}"
        echo "  Check: adb logcat -d | grep mondrian"
        exit 1
    fi

    # Read progress
    PROGRESS=$(adb shell run-as "$PKG" cat "$PROGRESS_FILE" 2>/dev/null || echo "0.000")
    PCT=$(python3 -c "print(min(int(float('${PROGRESS}') * 100), 100))" 2>/dev/null || echo "0")
    FILLED=$((PCT * BAR_WIDTH / 100))
    EMPTY=$((BAR_WIDTH - FILLED))
    BAR=$(printf '%0.s█' $(seq 1 $FILLED 2>/dev/null) 2>/dev/null)
    SPC=$(printf '%0.s░' $(seq 1 $EMPTY 2>/dev/null) 2>/dev/null)
    printf "\r  Running... [${BAR}${SPC}] %3d%%" "$PCT"

    if [ "$PROGRESS" = "1.000" ]; then
        break
    fi
done

printf "\r\033[K"
echo -e "  ${GREEN}Done! [$(printf '%0.s█' $(seq 1 $BAR_WIDTH))] 100%${NC}"
echo ""
echo "Run ./pull.sh to collect log files."
