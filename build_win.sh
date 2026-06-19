#!/bin/bash
# Build script: cross-compile HymnBookApp for Windows 64-bit and package as green zip
# Usage: ./build_win.sh

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/build_win"
QT_WIN="/home/claw/Qt/6.2.4/mingw_64"
NAME="HymnBookApp"
# 从 main.cpp 读取版本号
VERSION=$(grep 'setApplicationVersion' "${SCRIPT_DIR}/src/main.cpp" | grep -oP '"[^"]+"' | head -1 | tr -d '"')
echo "Version: ${VERSION}"

echo "=== Step 1/4: Configure CMake for Windows cross-compilation ==="
rm -rf "${BUILD_DIR}"
cmake -S "${SCRIPT_DIR}" -B "${BUILD_DIR}" \
    -DCMAKE_TOOLCHAIN_FILE="${SCRIPT_DIR}/cmake/toolchain-win64.cmake" \
    -DCMAKE_BUILD_TYPE=Release \
    -DQT_QMAKE_EXECUTABLE="${QT_WIN}/bin/qmake"

echo ""
echo "=== Step 2/4: Build ==="
cmake --build "${BUILD_DIR}" --parallel "$(nproc)"

# Verify the exe was created
EXE="${BUILD_DIR}/${NAME}.exe"
if [ ! -f "${EXE}" ]; then
    echo "ERROR: ${EXE} not found!"
    exit 1
fi
echo "Build succeeded: ${EXE}"

echo ""
echo "=== Step 3/4: Collect dependencies ==="
DEPLOY_DIR="${BUILD_DIR}/deploy/${NAME}"
rm -rf "${BUILD_DIR}/deploy"
mkdir -p "${DEPLOY_DIR}"

# Copy exe
cp "${EXE}" "${DEPLOY_DIR}/"

# Create qt.conf to ensure plugin paths are correct
cat > "${DEPLOY_DIR}/qt.conf" << EOF
[Paths]
Prefix = .
Plugins = plugins
EOF

# Copy data (empty JSON for fresh start)
cp -r "${SCRIPT_DIR}/data" "${DEPLOY_DIR}/data"
# Create empty images directory for first-run
mkdir -p "${DEPLOY_DIR}/images"

# Collect Qt DLLs
echo "  Collecting Qt DLLs..."
QT_BIN="${QT_WIN}/bin"
for dll in \
    Qt6Core Qt6Gui Qt6Widgets \
    ; do
    f="${QT_BIN}/${dll}.dll"
    if [ -f "$f" ]; then
        cp "$f" "${DEPLOY_DIR}/"
        echo "    ${dll}.dll"
    fi
done

# Collect MinGW runtime DLLs (use Qt-bundled MinGW 11.2, matching Qt build)
echo "  Collecting MinGW runtime DLLs (from Qt bundle)..."
for dll in libgcc_s_seh-1.dll libstdc++-6.dll libwinpthread-1.dll; do
    f="${QT_WIN}/bin/${dll}"
    if [ -f "$f" ]; then
        cp "$f" "${DEPLOY_DIR}/"
        echo "    ${dll} (Qt-bundled)"
    fi
done

# Strip debug info from MinGW DLLs to reduce size
echo "  Stripping debug symbols..."
x86_64-w64-mingw32-strip --strip-unneeded "${DEPLOY_DIR}/lib*.dll" 2>/dev/null || true
x86_64-w64-mingw32-strip --strip-unneeded "${DEPLOY_DIR}/Qt6*.dll" 2>/dev/null || true

# D3D compiler (Qt ANGLE fallback, needed for DirectX rendering)
if [ -f "${QT_WIN}/bin/d3dcompiler_47.dll" ]; then
    cp "${QT_WIN}/bin/d3dcompiler_47.dll" "${DEPLOY_DIR}/"
    echo "    d3dcompiler_47.dll"
fi

# Collect Qt plugins
echo "  Collecting Qt plugins..."
mkdir -p "${DEPLOY_DIR}/platforms"
mkdir -p "${DEPLOY_DIR}/styles"
mkdir -p "${DEPLOY_DIR}/imageformats"

QT_PLUGINS="${QT_WIN}/plugins"
if [ -d "${QT_PLUGINS}/platforms" ]; then
    cp "${QT_PLUGINS}/platforms/qwindows.dll" "${DEPLOY_DIR}/platforms/"
fi
if [ -d "${QT_PLUGINS}/styles" ]; then
    cp "${QT_PLUGINS}/styles/"*.dll "${DEPLOY_DIR}/styles/" 2>/dev/null || true
fi
if [ -d "${QT_PLUGINS}/imageformats" ]; then
    cp "${QT_PLUGINS}/imageformats/"*.dll "${DEPLOY_DIR}/imageformats/" 2>/dev/null || true
fi
if [ -d "${QT_PLUGINS}/iconengines" ]; then
    mkdir -p "${DEPLOY_DIR}/iconengines"
    cp "${QT_PLUGINS}/iconengines/"*.dll "${DEPLOY_DIR}/iconengines/" 2>/dev/null || true
fi

echo ""
echo "=== Step 4/4: Package green zip ==="
ZIP_NAME="${NAME}-${VERSION}-Windows-x86_64.zip"
cd "${BUILD_DIR}/deploy"
rm -f "${SCRIPT_DIR}/${ZIP_NAME}"
zip -r "${SCRIPT_DIR}/${ZIP_NAME}" "${NAME}"
echo ""
echo "=== Done! ==="
echo "Package: ${SCRIPT_DIR}/${ZIP_NAME}"
echo "Size:    $(du -h "${SCRIPT_DIR}/${ZIP_NAME}" | cut -f1)"
echo ""
echo "Top-level contents:"
unzip -l "${SCRIPT_DIR}/${ZIP_NAME}" | head -15
echo "  ..."
echo "  $(unzip -l "${SCRIPT_DIR}/${ZIP_NAME}" | tail -1)"
