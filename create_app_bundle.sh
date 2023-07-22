#!/bin/bash
# Create a self-contained macOS .app bundle for Breakout

set -e

APP_NAME="Breakout"
APP_BUNDLE="./build/${APP_NAME}.app"
CONTENTS="${APP_BUNDLE}/Contents"
MACOS="${CONTENTS}/MacOS"
FRAMEWORKS="${CONTENTS}/Frameworks"
RESOURCES="${CONTENTS}/Resources"
EXECUTABLE="breakout"

# Build type (default: release)
BUILD_TYPE="${1:-release}"

echo "Building ${BUILD_TYPE} version first..."
make "${BUILD_TYPE}"

echo ""
echo "Creating ${APP_NAME}.app bundle..."

# Clean previous bundle
rm -rf "${APP_BUNDLE}"

# Create directory structure
mkdir -p "${MACOS}"
mkdir -p "${FRAMEWORKS}"
mkdir -p "${RESOURCES}"

# Copy executable
cp "./build/${EXECUTABLE}" "${MACOS}/${APP_NAME}"

# Find and copy all required dylibs
copy_dylib() {
    local lib_path="$1"
    local lib_name=$(basename "$lib_path")

    if [[ ! -f "${FRAMEWORKS}/${lib_name}" ]]; then
        echo "  Copying: ${lib_name}"
        cp "$lib_path" "${FRAMEWORKS}/"
        chmod 755 "${FRAMEWORKS}/${lib_name}"

        # Recursively copy dependencies
        otool -L "$lib_path" | tail -n +2 | awk '{print $1}' | while read dep; do
            # Skip system libraries and self-references
            if [[ "$dep" != /usr/lib/* && "$dep" != /System/* && "$dep" != @rpath/* && "$dep" != @executable_path/* && -f "$dep" ]]; then
                copy_dylib "$dep"
            fi
        done
    fi
}

echo "Copying libraries..."

# Copy main SDL libraries
for lib in /usr/local/lib/libSDL3.0.dylib /usr/local/lib/libSDL3_ttf.0.dylib /usr/local/lib/libSDL3_mixer.0.dylib; do
    if [[ -f "$lib" ]]; then
        copy_dylib "$lib"
    fi
done

# Copy homebrew dependencies (harfbuzz, freetype, etc.)
for lib in /opt/homebrew/opt/harfbuzz/lib/libharfbuzz.0.dylib \
           /opt/homebrew/opt/freetype/lib/libfreetype.6.dylib \
           /opt/homebrew/opt/graphite2/lib/libgraphite2.3.dylib \
           /opt/homebrew/opt/glib/lib/libglib-2.0.0.dylib \
           /opt/homebrew/opt/gettext/lib/libintl.8.dylib \
           /opt/homebrew/opt/pcre2/lib/libpcre2-8.0.dylib \
           /opt/homebrew/opt/libpng/lib/libpng16.16.dylib \
           /opt/homebrew/opt/brotli/lib/libbrotlidec.1.dylib \
           /opt/homebrew/opt/brotli/lib/libbrotlicommon.1.dylib; do
    if [[ -f "$lib" ]]; then
        copy_dylib "$lib"
    fi
done

echo "Fixing library paths..."

# Fix library paths in all dylibs
fix_dylib_paths() {
    local file="$1"
    local file_name=$(basename "$file")

    # Change the library's own ID
    install_name_tool -id "@executable_path/../Frameworks/${file_name}" "$file" 2>/dev/null || true

    # Fix all dependencies
    otool -L "$file" | tail -n +2 | awk '{print $1}' | while read dep; do
        local dep_name=$(basename "$dep")

        # Fix @rpath references
        if [[ "$dep" == @rpath/* ]]; then
            install_name_tool -change "$dep" "@executable_path/../Frameworks/${dep_name}" "$file" 2>/dev/null || true
        fi

        # Fix absolute paths to our bundled libs
        if [[ -f "${FRAMEWORKS}/${dep_name}" ]]; then
            install_name_tool -change "$dep" "@executable_path/../Frameworks/${dep_name}" "$file" 2>/dev/null || true
        fi
    done
}

# Fix the main executable
echo "  Fixing: ${APP_NAME}"
fix_dylib_paths "${MACOS}/${APP_NAME}"

# Fix all framework libraries
for lib in "${FRAMEWORKS}"/*.dylib; do
    if [[ -f "$lib" ]]; then
        echo "  Fixing: $(basename "$lib")"
        fix_dylib_paths "$lib"
    fi
done

# Create Info.plist
cat > "${CONTENTS}/Info.plist" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleDevelopmentRegion</key>
    <string>en</string>
    <key>CFBundleExecutable</key>
    <string>${APP_NAME}</string>
    <key>CFBundleIdentifier</key>
    <string>com.breakout.game</string>
    <key>CFBundleInfoDictionaryVersion</key>
    <string>6.0</string>
    <key>CFBundleName</key>
    <string>${APP_NAME}</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>CFBundleShortVersionString</key>
    <string>1.0</string>
    <key>CFBundleVersion</key>
    <string>1</string>
    <key>LSMinimumSystemVersion</key>
    <string>11.0</string>
    <key>NSHighResolutionCapable</key>
    <true/>
    <key>NSPrincipalClass</key>
    <string>NSApplication</string>
</dict>
</plist>
EOF

# Create PkgInfo
echo -n "APPL????" > "${CONTENTS}/PkgInfo"

# Remove quarantine and sign ad-hoc
echo ""
echo "Code signing..."
xattr -cr "${APP_BUNDLE}"
codesign --force --deep --sign - "${APP_BUNDLE}"

echo ""
echo "Verifying library dependencies..."
otool -L "${MACOS}/${APP_NAME}"

echo ""
echo "Bundle contents:"
find "${APP_BUNDLE}" -type f | head -20

echo ""
echo "Bundle size:"
du -sh "${APP_BUNDLE}"

echo ""
echo "✅ Created: ${APP_BUNDLE}"
echo ""
echo "To test: open ${APP_BUNDLE}"
echo "To distribute: zip -r Breakout.zip ${APP_BUNDLE}"
