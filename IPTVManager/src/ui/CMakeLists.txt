set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_library(iptv_ui STATIC
    mainwindow.cpp
    mainwindow.ui
    dialogs/dbsetupdialog.cpp
    dialogs/dbsetupdialog.ui
    dialogs/infodialog.cpp
    dialogs/infodialog.ui
    widgets/video_preview_widget.cpp
    models/channel_table_model.cpp
    models/source_table_model.cpp
)

target_include_directories(iptv_ui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(iptv_ui PUBLIC
    Qt::Core Qt::Widgets Qt::Sql Qt::OpenGLWidgets Qt::Network
    iptv_core iptv_database iptv_network iptv_multimedia iptv_export iptv_logic
)
