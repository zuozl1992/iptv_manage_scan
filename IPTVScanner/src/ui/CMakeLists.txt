# 用户界面模块 - 主窗口和组件
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(iptv_ui STATIC
    mainwindow.cpp
    mainwindow.ui
    models/scan_result_model.cpp
    widgets/log_widget.cpp
)

target_include_directories(iptv_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(iptv_ui PUBLIC
    Qt::Core Qt::Widgets
    iptv_core iptv_multimedia iptv_logic
)
