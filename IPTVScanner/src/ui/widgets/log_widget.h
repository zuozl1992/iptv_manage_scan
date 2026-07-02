#pragma once

#include <QTextBrowser>
#include <QWidget>

namespace Iptv::Ui {

class LogWidget : public QWidget
{
    Q_OBJECT

public:
    explicit LogWidget(QWidget *parent = nullptr);

    void append(const QString &text);
    void clear();
    QString toPlainText() const;

private:
    QTextBrowser *m_textBrowser;
};

} // namespace Iptv::Ui
