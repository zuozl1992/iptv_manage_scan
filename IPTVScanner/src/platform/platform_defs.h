#pragma once

#ifdef IPTV_OS_WINDOWS
    #define IPTV_PLATFORM_WINDOWS
#elif defined(IPTV_OS_MACOS)
    #define IPTV_PLATFORM_MACOS
#elif defined(IPTV_OS_LINUX)
    #define IPTV_PLATFORM_LINUX
#endif

// Platform-specific includes
#ifdef IPTV_OS_WINDOWS
    #include <windows.h>
#endif
