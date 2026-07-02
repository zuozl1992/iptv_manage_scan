# 核心模块 - 配置管理和日志系统
add_library(iptv_core STATIC
    config/appconfig.cpp
    logging/logmanager.cpp
)

target_include_directories(iptv_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(iptv_core PUBLIC Qt::Core)
