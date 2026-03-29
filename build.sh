#!/bin/bash
# build.sh - Bootstrap premake5 from the OPTKIT submodule and generate build files

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
PREMAKE5_SRC="$SCRIPT_DIR/lib/OPTKIT/lib/premake5"
PREMAKE5_BIN="$SCRIPT_DIR/lib/OPTKIT/lib/premake5/bin/release/premake5"

# ────────────────────────────────────────────────
# 1. Initialize required submodules
# ────────────────────────────────────────────────
echo ">>> Initializing submodules..."
git -C "$SCRIPT_DIR" submodule update --init --recursive --force --remote lib/OPTKIT
git -C "$SCRIPT_DIR" submodule update --init --recursive --force --remote lib/imgui
git -C "$SCRIPT_DIR" submodule update --init --recursive --force --remote lib/implot
git -C "$SCRIPT_DIR" submodule update --init --recursive --force --remote lib/glfw

# ────────────────────────────────────────────────
# 2. Build premake5 from source (if not already built)
# ────────────────────────────────────────────────
if [ ! -f "$PREMAKE5_BIN" ]; then
    echo ">>> Building premake5 from source..."
    SYSTEM=$(uname -s)
    if [ "$SYSTEM" = "Linux" ]; then
        make -C "$PREMAKE5_SRC" -f Bootstrap.mak linux
    else
        echo "Unsupported platform: $SYSTEM (only Linux is supported)"; exit 1
    fi
else
    echo ">>> premake5 binary already exists, skipping build."
fi

# ────────────────────────────────────────────────
# 3. Building OPTKIT
# ────────────────────────────────────────────────
echo ">>> Building OPTKIT..."
cd "$SCRIPT_DIR/lib/OPTKIT"
premake5 gmake
make -j$(nproc) config=release optkit_dynamic


# ────────────────────────────────────────────────
# 4. Run premake5 with the requested action
# ────────────────────────────────────────────────
ACTION="${1:-gmake2}"
echo ">>> Running premake5 $ACTION ..."
cd "$SCRIPT_DIR"
"$PREMAKE5_BIN" "$ACTION"

echo ""
echo "Done! To build the project:"
echo "  make [config=debug|release]"
