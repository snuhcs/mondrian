#!/bin/bash
set -e

# =============================================================================
# Mondrian Setup Script
# Automates: Conda env, model export, Android SDK, native deps, APK build
# =============================================================================

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

step_num=0
total_steps=5

print_step() {
    step_num=$((step_num + 1))
    echo ""
    echo -e "${GREEN}[$step_num/$total_steps] $1${NC}"
    echo "-----------------------------------------------"
}

print_skip() {
    echo -e "${YELLOW}  -> Already done. Skipping.${NC}"
}

print_error() {
    echo -e "${RED}ERROR: $1${NC}" >&2
}

# =============================================================================
# Pre-flight checks
# =============================================================================
echo "Checking prerequisites..."

if ! command -v java &> /dev/null; then
    print_error "Java not found. Install JDK 17+:"
    echo "  sudo apt install openjdk-17-jdk"
    exit 1
fi

JAVA_VER=$(java -version 2>&1 | head -1 | grep -oP '\"(\d+)' | tr -d '"')
if [ "$JAVA_VER" -lt 17 ] 2>/dev/null; then
    print_error "Java 17+ required (found Java $JAVA_VER)."
    echo "  sudo apt install openjdk-17-jdk"
    exit 1
fi

if ! command -v conda &> /dev/null; then
    print_error "Conda not found. Install Miniconda:"
    echo "  https://docs.conda.io/en/latest/miniconda.html"
    exit 1
fi

echo -e "${GREEN}Prerequisites OK.${NC}"

# =============================================================================
# Step 1: Conda environment + TFLite model export
# =============================================================================
print_step "Setting up conda environment and exporting TFLite models"

# Create conda env if it doesn't exist
if conda env list | grep -q "mondrian-offline"; then
    echo "  Conda environment 'mondrian-offline' already exists."
else
    echo "  Creating conda environment 'mondrian-offline'..."
    conda env create -f offline/environment.yml
fi

# Export models (check each individually)
MODELS_TO_EXPORT=(
    "yolov5l.pt:1024:yolov5l-1024-fp16.tflite"
    "yolov5m.pt:640:yolov5m-640-fp16.tflite"
    "yolov5m.pt:1280:yolov5m-1280-fp16.tflite"
)

all_models_exist=true
for entry in "${MODELS_TO_EXPORT[@]}"; do
    IFS=':' read -r model imgsz output <<< "$entry"
    if [ ! -f "$output" ]; then
        all_models_exist=false
        break
    fi
done

if $all_models_exist; then
    echo "  All TFLite models already exist."
    print_skip
else
    echo "  Exporting TFLite models (this may take a while)..."
    for entry in "${MODELS_TO_EXPORT[@]}"; do
        IFS=':' read -r model imgsz output <<< "$entry"
        if [ -f "$output" ]; then
            echo "  $output already exists. Skipping."
        else
            echo "  Exporting $model (imgsz=$imgsz)..."
            YOLO_AUTOINSTALL=false \
                conda run -n mondrian-offline \
                python offline/export_tflite.py --model "$model" --imgsz "$imgsz"
        fi
    done
    echo -e "${GREEN}  Model export complete.${NC}"
fi

# =============================================================================
# Step 2: Android SDK setup
# =============================================================================
print_step "Setting up Android SDK"

export ANDROID_HOME="${ANDROID_HOME:-$HOME/Android/Sdk}"

if [ -d "$ANDROID_HOME/platform-tools" ] && \
   [ -d "$ANDROID_HOME/platforms/android-30" ] && \
   [ -d "$ANDROID_HOME/build-tools/30.0.3" ] && \
   [ -d "$ANDROID_HOME/ndk/21.4.7075529" ] && \
   [ -d "$ANDROID_HOME/cmake/3.18.1" ]; then
    echo "  All required SDK packages already installed."
    print_skip
else
    # Check if cmdline-tools exist
    SDKMANAGER=""
    if [ -f "$ANDROID_HOME/cmdline-tools/latest/bin/sdkmanager" ]; then
        SDKMANAGER="$ANDROID_HOME/cmdline-tools/latest/bin/sdkmanager"
    elif command -v sdkmanager &> /dev/null; then
        SDKMANAGER="sdkmanager"
    fi

    if [ -z "$SDKMANAGER" ]; then
        # Download cmdline-tools
        echo "  Downloading Android command-line tools..."
        mkdir -p "$ANDROID_HOME/cmdline-tools"
        CMDLINE_TOOLS_URL="https://dl.google.com/android/repository/commandlinetools-linux-11076708_latest.zip"
        curl -fSL -o /tmp/cmdline-tools.zip "$CMDLINE_TOOLS_URL"
        unzip -qo /tmp/cmdline-tools.zip -d /tmp/cmdline-tools-tmp
        rm -rf "$ANDROID_HOME/cmdline-tools/latest"
        mv /tmp/cmdline-tools-tmp/cmdline-tools "$ANDROID_HOME/cmdline-tools/latest"
        rm -f /tmp/cmdline-tools.zip
        rm -rf /tmp/cmdline-tools-tmp
        SDKMANAGER="$ANDROID_HOME/cmdline-tools/latest/bin/sdkmanager"
    fi

    echo "  Installing SDK packages..."
    yes | "$SDKMANAGER" \
        "platform-tools" \
        "platforms;android-30" \
        "build-tools;30.0.3" \
        "ndk;21.4.7075529" \
        "cmake;3.18.1" \
        > /dev/null 2>&1 || true

    echo "  Accepting licenses..."
    yes | "$SDKMANAGER" --licenses > /dev/null 2>&1 || true

    echo -e "${GREEN}  Android SDK setup complete.${NC}"
fi

export PATH="$ANDROID_HOME/cmdline-tools/latest/bin:$ANDROID_HOME/platform-tools:$PATH"

# =============================================================================
# Step 3: Native dependencies
# =============================================================================
print_step "Setting up native dependencies"

DEPS_DIR="android/Mondrian/src/main/cpp"

if [ -d "$DEPS_DIR/jniLibs/arm64-v8a" ] && [ -d "$DEPS_DIR/third_party/include/opencv2" ]; then
    echo "  Native dependencies already exist."
    print_skip
else
    ZIP_PATH="android/mondrian-dependencies.zip"
    if [ ! -f "$ZIP_PATH" ]; then
        print_error "mondrian-dependencies.zip not found at $ZIP_PATH"
        echo ""
        echo "  Download it from OneDrive and place it in android/:"
        echo "    https://1drv.ms/u/c/37b8a46db9d487d9/IQC1d7OHyUsgRYAUEit4uQ8MAbNHepXr5sJAlwkK_j4Vu1k?e=kc65q4"
        echo ""
        echo "  Then re-run this script."
        exit 1
    fi

    echo "  Extracting dependencies..."
    unzip -o "$ZIP_PATH" -d "$DEPS_DIR/"
    rm "$ZIP_PATH"
    echo -e "${GREEN}  Native dependencies installed.${NC}"
fi

# =============================================================================
# Step 4: Build APK
# =============================================================================
print_step "Building APK"

"$SCRIPT_DIR/build.sh"

# =============================================================================
# Step 5: Push models and scale estimator to device
# =============================================================================
print_step "Pushing models and scale estimator to device"

if ! command -v adb &> /dev/null; then
    print_error "adb not found. Make sure a device is connected and ADB is in PATH."
    echo "  You can manually push later with run.sh."
    echo ""
else
    if adb devices | grep -q "device$"; then
        adb shell mkdir -p /data/local/tmp/models/

        echo "  Pushing TFLite models..."
        adb push yolov5l-1024-fp16.tflite /data/local/tmp/models/
        adb push yolov5m-640-fp16.tflite /data/local/tmp/models/
        adb push yolov5m-1280-fp16.tflite /data/local/tmp/models/

        echo "  Pushing scale estimator..."
        adb push offline/scale_estimator_mta.json /data/local/tmp/scale_estimator.json 2>/dev/null || true

        echo -e "${GREEN}  Models pushed to device.${NC}"
    else
        echo -e "${YELLOW}  No device connected. Skipping model push.${NC}"
        echo "  Connect a device and run: ./run.sh"
    fi
fi

# =============================================================================
# Done
# =============================================================================
echo ""
echo "========================================="
echo -e "${GREEN}Setup complete!${NC}"
echo "========================================="
echo ""
echo "Next steps:"
echo "  1. Push your video to the device:"
echo "       adb push <video_file> /data/local/tmp/video/"
echo "  2. Edit android/config.json (set video path, num_frames, fps, etc.)"
echo "  3. Push config and run:"
echo "       adb push android/config.json /data/local/tmp/config.json"
echo "       ./run.sh"
echo ""
