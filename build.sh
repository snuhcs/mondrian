#!/bin/bash
set -e

# =============================================================================
# Mondrian Build Script
# Incremental APK build for use after code changes
# =============================================================================

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

RED='\033[0;31m'
GREEN='\033[0;32m'
NC='\033[0m'

export ANDROID_HOME="${ANDROID_HOME:-$HOME/Android/Sdk}"

if [ ! -f "$SCRIPT_DIR/android/local.properties" ]; then
    echo "sdk.dir=$ANDROID_HOME" > "$SCRIPT_DIR/android/local.properties"
fi

echo "Building APK..."
cd "$SCRIPT_DIR/android"
./gradlew assembleDebug

APK_PATH="Mondrian/build/outputs/apk/debug/Mondrian-debug.apk"
if [ -f "$APK_PATH" ]; then
    echo -e "${GREEN}APK built: android/$APK_PATH${NC}"
else
    echo -e "${RED}Build failed.${NC}"
    exit 1
fi
