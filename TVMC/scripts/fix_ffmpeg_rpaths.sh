#!/bin/bash
# Fix FFmpeg library paths in app bundle
APP_BUNDLE="$1"
FFMPEG_LIB_PATH="$2"
APP_EXEC="$APP_BUNDLE/Contents/MacOS/IPTVManager"

if [ ! -d "$APP_BUNDLE/Contents/Frameworks" ]; then
    exit 0
fi

# Fix install names for ffmpeg libraries
for lib in "$APP_BUNDLE"/Contents/Frameworks/libav*.dylib "$APP_BUNDLE"/Contents/Frameworks/libsw*.dylib; do
    if [ -f "$lib" ]; then
        libname=$(basename "$lib")
        install_name_tool -id "@executable_path/../Frameworks/$libname" "$lib" 2>/dev/null || true
        
        # Update executable to use bundled library
        install_name_tool -change "$FFMPEG_LIB_PATH/$libname" "@executable_path/../Frameworks/$libname" "$APP_EXEC" 2>/dev/null || true
    fi
done
