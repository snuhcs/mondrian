#!/bin/bash
set -e

# =============================================================================
# Mondrian Log Pull Script
# Pulls log files from the device to the local logs/ directory
# =============================================================================

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PKG="hcs.offloading.mondrian"
LOCAL_LOGS="$SCRIPT_DIR/logs"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

if ! adb devices | grep -q "device$"; then
    echo -e "${RED}ERROR: No device connected.${NC}"
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

# Check if logs exist on device
if ! adb shell run-as "$PKG" ls log/ > /dev/null 2>&1; then
    echo -e "${RED}ERROR: No log directory found on device.${NC}"
    echo "  Run the app first with ./run.sh"
    exit 1
fi

# List available runs
echo "Available runs on device:"
RUNS=$(adb shell run-as "$PKG" ls log/ 2>/dev/null)
for run in $RUNS; do
    PROGRESS=$(adb shell run-as "$PKG" cat "log/$run/progress" 2>/dev/null || echo "?")
    if [ "$PROGRESS" = "1.000" ]; then
        STATUS="${GREEN}complete${NC}"
    elif [ "$PROGRESS" = "?" ]; then
        STATUS="${YELLOW}unknown${NC}"
    else
        STATUS="${YELLOW}${PROGRESS}${NC}"
    fi
    echo -e "  $run  ($STATUS)"
done

# Pull logs
echo ""
echo "Pulling logs..."
mkdir -p "$LOCAL_LOGS"
rm -rf "$LOCAL_LOGS/log"
adb shell run-as "$PKG" tar -cf - log/ | tar -xf - -C "$LOCAL_LOGS/"

# Move from logs/log/<run>/ to logs/<run>/ for cleaner structure
for run in "$LOCAL_LOGS"/log/*/; do
    run_name=$(basename "$run")
    rm -rf "$LOCAL_LOGS/$run_name"
    mv "$run" "$LOCAL_LOGS/$run_name"
done
rmdir "$LOCAL_LOGS/log" 2>/dev/null || true

echo -e "${GREEN}Done.${NC}"
echo ""

# Print summary
for run_dir in "$LOCAL_LOGS"/*/; do
    [ -d "$run_dir" ] || continue
    run_name=$(basename "$run_dir")
    echo "  $LOCAL_LOGS/$run_name/"
    for f in "$run_dir"*; do
        [ -f "$f" ] || continue
        fname=$(basename "$f")
        fsize=$(du -h "$f" | cut -f1)
        printf "    %-20s %s\n" "$fname" "$fsize"
    done
    echo ""
done
