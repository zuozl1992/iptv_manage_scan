#include "video_preview_widget.h"
#include <QPainter>
#include <QImage>

namespace Iptv::Ui {

VideoPreviewWidget::VideoPreviewWidget(QWidget *parent)
    : QOpenGLWidget(parent)
{
}

VideoPreviewWidget::~VideoPreviewWidget()
{
    clearPage();
}

void VideoPreviewWidget::setData(uchar *data, int width, int height)
{
    clearPage();
    m_data = data;
    m_width = width;
    m_height = height;
    update();
}

void VideoPreviewWidget::clearPage()
{
    if (m_data) {
        delete[] m_data;
        m_data = nullptr;
    }
    m_width = 0;
    m_height = 0;
    update();
}

void VideoPreviewWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    
    QPainter painter(this);
    
    if (!m_data || m_width <= 0 || m_height <= 0) {
        painter.fillRect(rect(), Qt::black);
        return;
    }
    
    // Create QImage from BGRA data
    QImage image(m_data, m_width, m_height, m_width * 4, QImage::Format_ARGB32);
    
    // Scale to widget size
    QPixmap pixmap = QPixmap::fromImage(image);
    QPixmap scaled = pixmap.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
    
    // Center the image
    int x = (width() - scaled.width()) / 2;
    int y = (height() - scaled.height()) / 2;
    
    painter.fillRect(rect(), Qt::black);
    painter.drawPixmap(x, y, scaled);
}

void VideoPreviewWidget::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    update();
}

} // namespace Iptv::Ui
