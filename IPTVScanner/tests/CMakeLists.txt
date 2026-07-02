enable_testing()
find_package(Qt6 REQUIRED COMPONENTS Test)

add_executable(iptvscanner_tests
    test_url_builder.cpp
)

target_link_libraries(iptvscanner_tests PRIVATE
    Qt::Test
    iptv_logic
    iptv_core
)

add_test(NAME iptvscanner_tests COMMAND iptvscanner_tests)
