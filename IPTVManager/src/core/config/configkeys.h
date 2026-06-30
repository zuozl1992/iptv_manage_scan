#pragma once

#include <QString>

namespace Iptv::Core::ConfigKeys {

// Stream URL settings
constexpr const char* HTTP_URL = "set/http_url";
constexpr const char* FILE_URL = "set/file_url";
constexpr const char* FCC_URL = "set/fcc_url";
constexpr const char* FCC_USE = "set/fcc_use";

// Group/scan settings
constexpr const char* GROUP_URL = "set/group_url";
constexpr const char* GROUP_PORT = "set/group_port";
constexpr const char* GROUP_TYPE = "set/group_type";
constexpr const char* GROUP_ADD_TEST = "set/group_add_test";

// Export settings
constexpr const char* OUT_BY_ONE = "set/out_by_one";
constexpr const char* TV_GROUP_SELECT = "set/tv_group_select";
constexpr const char* TV_MERGE = "set/tv_merge";
constexpr const char* OUT_ADD_LOGO = "set/out_add_logo";
constexpr const char* OUT_ADD_HD = "set/out_add_hd";

// Default values
constexpr const char* DEFAULT_HTTP_URL = "http://192.168.1.1:12345";
constexpr const char* DEFAULT_FILE_URL = "http://192.168.1.1:54321";
constexpr const char* DEFAULT_GROUP_URL = "239.49.{0#1#2}";
constexpr const char* DEFAULT_GROUP_PORT = "6000#8000#8008";

} // namespace Iptv::Core::ConfigKeys
