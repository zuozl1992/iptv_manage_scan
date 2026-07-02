# 业务逻辑模块 - URL展开和扫描服务
add_library(iptv_logic STATIC
    url_builder.cpp
    scan_service.cpp
)

target_include_directories(iptv_logic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(iptv_logic PUBLIC Qt::Core iptv_core iptv_multimedia)
