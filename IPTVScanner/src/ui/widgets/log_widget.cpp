#include "log_widget.h"

#include <QVBoxLayout>

namespace Iptv::Ui {

LogWidget::LogWidget(QWidget *parent)
    : QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    m_textBrowser = new QTextBrowser(this);
    layout->addWidget(m_textBrowser);
}

void LogWidget::append(const QString &text)
{
    m_textBrowser->append(text);
}

void LogWidget::clear()
{
    m_textBrowser->clear();
}

QString LogWidget::toPlainText() const
{
    return m_textBrowser->toPlainText();
}

} // namespace Iptv::Ui
