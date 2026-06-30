enable_testing()

find_package(Qt6 REQUIRED COMPONENTS Test)

# Test URL Builder
add_executable(test_url_builder test_url_builder.cpp)
target_link_libraries(test_url_builder PRIVATE Qt::Core Qt::Test iptv_logic)
add_test(NAME test_url_builder COMMAND test_url_builder)

# Test Channel Service
add_executable(test_channel_service test_channel_service.cpp)
target_link_libraries(test_channel_service PRIVATE Qt::Core Qt::Test iptv_logic iptv_database)
add_test(NAME test_channel_service COMMAND test_channel_service)

# Test Source Repository
add_executable(test_source_repository test_source_repository.cpp)
target_link_libraries(test_source_repository PRIVATE Qt::Core Qt::Sql Qt::Test iptv_database)
add_test(NAME test_source_repository COMMAND test_source_repository)

# Test Exporters
add_executable(test_exporters test_exporters.cpp)
target_link_libraries(test_exporters PRIVATE Qt::Core Qt::Test iptv_export QXlsx)
add_test(NAME test_exporters COMMAND test_exporters)

# Test Config
add_executable(test_config test_config.cpp)
target_link_libraries(test_config PRIVATE Qt::Core Qt::Test iptv_core)
add_test(NAME test_config COMMAND test_config)
