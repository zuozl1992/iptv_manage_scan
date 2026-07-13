#pragma once

#include <QString>
#include <QStringList>

namespace Iptv::Logic {

/**
 * @brief URL构建工具类
 *        负责解析组播地址模板并生成扫描/测试URL
 */
class UrlBuilder
{
public:
    /** @brief 展开地址模板中的花括号表达式
     *  @param templ 包含{...}的模板字符串，支持#分隔和[start-end]范围
     *  @return 展开后的字符串列表 */
    static QStringList expand(const QString &templ);

    /** @brief 构建待扫描的IP地址列表
     *  @param groupAddress 组播地址模板（如239.3.5.{[1-10]}）
     *  @param port         端口号
     *  @param existingIps  数据库中已存在的IP列表（用于过滤）
     *  @return 格式为ip:{port}的扫描地址列表 */
    static QStringList buildScanIps(const QString &groupAddress,
                                    const QString &port,
                                    const QStringList &existingIps);

    /** @brief 构建测试频道URL列表
     *  @param port        端口号
     *  @param existingIps 已存在的IP列表
     *  @return 测试频道URL列表 */
    static QStringList buildTestUrls(const QString &port,
                                     const QStringList &existingIps);

private:
    /** @brief 展开单个范围表达式
     *  @param pattern 花括号内的内容（如"1#2#3"或"1-255"）
     *  @return 展开后的值列表 */
    static QStringList expandRange(const QString &pattern);
};

} // namespace Iptv::Logic
