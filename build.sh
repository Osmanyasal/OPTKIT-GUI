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
git -C "$SCRIPT_DIR" submodule update --init lib/OPTKIT
git -C "$SCRIPT_DIR/lib/OPTKIT" submodule update --init lib/premake5
git -C "$SCRIPT_DIR" submodule update --init lib/imgui
git -C "$SCRIPT_DIR" submodule update --init lib/implot
git -C "$SCRIPT_DIR" submodule update --init lib/glfw

# ────────────────────────────────────────────────
# 2. Build premake5 from source (if not already built)
# ────────────────────────────────────────────────
if [ ! -f "$PREMAKE5_BIN" ]; then
    echo ">>> Building premake5 from source..."
    SYSTEM=$(uname -s)
    case "$SYSTEM" in
        Linux)  make -C "$PREMAKE5_SRC" -f Bootstrap.mak linux ;;
        Darwin) make -C "$PREMAKE5_SRC" -f Bootstrap.mak osx ;;
        *)      echo "Unsupported platform: $SYSTEM"; exit 1 ;;
    esac
else
    echo ">>> premake5 binary already exists, skipping build."
fi

# ────────────────────────────────────────────────
# 3. Run premake5 with the requested action
# ────────────────────────────────────────────────
ACTION="${1:-gmake2}"
echo ">>> Running premake5 $ACTION ..."
cd "$SCRIPT_DIR"
"$PREMAKE5_BIN" "$ACTION"

echo ""
echo "Done! To build the project:"
echo "  make [config=debug|release]"
