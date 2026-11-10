#!/usr/bin/env bash
# Copyright 2026 AislanJefferson. Licensed under the Apache License, Version 2.0.
# ---------------------------------------------------------------------------
# Build everything in one go: firmware, overlay apps (.app) and the matching
# CHIRP module, collected into build/dist/.
#
# Usage:
#   ./build-all.sh
#
# Steps run one after the other (both build scripts share the Docker image
# and build/, so they must not run in parallel):
#   1. firmware  -> ./compile-with-docker.sh, then the flash budget check
#   2. apps      -> ./compile-app.sh (every app)
#   3. chirp     -> picks KLEANFW_<X.Y.Z>_chirp_module.py for the firmware
#                   version and checks that it parses
# Exits non-zero if any step fails; the other steps still run.
# ---------------------------------------------------------------------------
set -uo pipefail

IMAGE=uvk1-uvk5v3
FW_DIR=build/Fusion
APPS_OUT=build/Apps
DIST=build/dist
FLASH_MAX=120832                    # firmware .bin limit (bytes)

cd "$(dirname "$0")"
export MSYS_NO_PATHCONV=1

command -v docker >/dev/null 2>&1 || { echo "❌ docker not found"; exit 1; }

fw_state="❌ FAIL"; apps_state="❌ FAIL"; chirp_state="❌ FAIL"; fail=0

rm -rf "$DIST" "$APPS_OUT"
mkdir -p "$DIST/Apps"

# --- 1. firmware ------------------------------------------------------------
echo "=== 1/3 Firmware ==="
fw_ver=""
if ./compile-with-docker.sh && [ -f "$FW_DIR/kleanfw.bin" ]; then
    size=$(wc -c < "$FW_DIR/kleanfw.bin" | tr -d ' ')
    cp -f "$FW_DIR/kleanfw.bin" "$FW_DIR/kleanfw.hex" "$DIST/"
    for mb in "$FW_DIR"/f4hwn.kleanfw.*.bin; do
        [ -f "$mb" ] || continue
        cp -f "$mb" "$DIST/"
        fw_ver=$(basename "$mb" | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -1)
    done
    if [ "$size" -le "$FLASH_MAX" ]; then
        fw_state="✅ OK  kleanfw.bin $size / $FLASH_MAX B ($((FLASH_MAX - size)) B free)"
    else
        fw_state="🚨 OVER BUDGET  kleanfw.bin $size / $FLASH_MAX B (+$((size - FLASH_MAX)) B)"
        fail=1
    fi
else
    fail=1
fi

# Version for the CHIRP module: from the multiboot image name, else the preset.
if [ -z "$fw_ver" ]; then
    fw_ver=$(grep -oE '"VERSION_STRING_2"[^"]*"v[0-9]+\.[0-9]+\.[0-9]+' CMakePresets.json \
             | grep -oE '[0-9]+\.[0-9]+\.[0-9]+' | head -1)
fi

# --- 2. apps ----------------------------------------------------------------
echo
echo "=== 2/3 Apps ==="
if ./compile-app.sh; then
    apps_state="✅ OK"
else
    apps_state="⚠️  some apps failed or overflowed (see table above)"
    fail=1
fi
n_apps=0
for f in "$APPS_OUT"/*.app; do
    [ -f "$f" ] || continue
    cp -f "$f" "$DIST/Apps/"; n_apps=$((n_apps + 1))
done
apps_state="$apps_state  ($n_apps .app)"

# --- 3. CHIRP module --------------------------------------------------------
# The module must match the firmware version exactly: an older module can
# overwrite newer channel fields, so there is no fallback to another version.
echo
echo "=== 3/3 CHIRP module ==="
chirp="KLEANFW_${fw_ver}_chirp_module.py"
if [ -z "$fw_ver" ]; then
    chirp_state="❌ firmware version unknown"; fail=1
elif [ ! -f "$chirp" ]; then
    chirp_state="❌ $chirp not found"; fail=1
elif docker run --rm -u "$(id -u):$(id -g)" -v "$PWD":/src -w /src "$IMAGE" \
        python3 -I -c 'import ast,sys; ast.parse(open(sys.argv[1], encoding="utf-8").read(), sys.argv[1])' \
        "$chirp"; then
    cp -f "$chirp" "$DIST/"
    chirp_state="✅ OK  $chirp"
else
    chirp_state="❌ $chirp does not parse"; fail=1
fi
echo "$chirp_state"

# --- summary ----------------------------------------------------------------
echo
echo "📦 Summary (v${fw_ver:-?})"
echo "   Firmware : $fw_state"
echo "   Apps     : $apps_state"
echo "   CHIRP    : $chirp_state"
echo
echo "📁 $DIST/"
(cd "$DIST" && find . -type f | sed 's|^\./|   |' | sort)
echo
if [ "$fail" -eq 0 ]; then
    echo "🎉 Everything built  ->  $DIST/"
else
    echo "⚠️  Some steps failed — see the summary above."
fi
exit $fail
