#pragma once

#include <QTextBrowser>
#include <QWidget>

namespace Iptv::Ui {

/**
 * @brief 日志显示控件
 *        封装QTextBrowser，提供日志文本的追加、清空和导出功能
 */
class LogWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LogWidget(QWidget *parent = nullptr);

    /** @brief 追加日志文本 */
    void append(const QString &text);

    /** @brief 清空日志 */
    void clear();

    /** @brief 获取纯文本内容 */
    QString toPlainText() const;

private:
    QTextBrowser *m_textBrowser;    ///< 文本浏览器
};

} // namespace Iptv::Ui
