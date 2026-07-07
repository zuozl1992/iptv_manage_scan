#pragma once

#include <QString>

namespace Iptv::Ui::Styles {

constexpr const char* LABEL_RED_STYLE =
    "QLabel { border: 1px solid red; border-radius: 2px; padding: 2px; }";      ///< 标签错误样式（红色边框）

constexpr const char* LABEL_NORMAL_STYLE =
    "QLabel { border: 1px solid gray; border-radius: 2px; padding: 2px; }";     ///< 标签正常样式（灰色边框）

constexpr const char* COMBO_RED_STYLE =
    "QComboBox { color: red; }";    ///< 下拉框错误样式（红色文字）

constexpr const char* COMBO_NORMAL_STYLE =
    "QComboBox { color: black; }";  ///< 下拉框正常样式（黑色文字）

} // namespace Iptv::Ui::Styles
