#pragma once

#include <QString>
#include <QStringList>

namespace Iptv::Logic {

/**
 * @brief URL构建工具类
 *        解析地址模板并展开为完整的URL列表
 */
class UrlBuilder
{
public:
    /** @brief 展开URL模板中的花括号表达式
     *  @param templ 包含{...}的模板字符串
     *  @return 展开后的URL列表 */
    static QStringList expand(const QString &templ);

    /** @brief 从文件读取URL模板并展开
     *  @param filePath 模板文件路径（每行一个模板）
     *  @return 展开后的URL列表 */
    static QStringList expandFromFile(const QString &filePath);

    /** @brief 从URL中提取IP地址
     *  @param url 完整URL（支持udp://、rtp://、http://.../udp/等格式）
     *  @return IP地址字符串 */
    static QString getIpFromUrl(const QString &url);

private:
    /** @brief 展开单个范围表达式
     *  @param pattern 花括号内的内容（如"1#3#[5-7]"或"[1-255]"）
     *  @return 展开后的值列表 */
    static QStringList expandRange(const QString &pattern);
};

} // namespace Iptv::Logic
