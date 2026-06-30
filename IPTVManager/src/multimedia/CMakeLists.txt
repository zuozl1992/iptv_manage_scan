add_library(iptv_multimedia STATIC
    stream_info.cpp
    video_decoder.cpp
    stream_probe.cpp
    ffmpeg_utils.cpp
)

target_include_directories(iptv_multimedia PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..)
target_link_libraries(iptv_multimedia PUBLIC
    Qt::Core
    iptv_core
    FFmpeg::avformat
    FFmpeg::avcodec
    FFmpeg::avutil
    FFmpeg::swscale
)
