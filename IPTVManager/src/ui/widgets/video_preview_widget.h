#pragma once

#include <QOpenGLWidget>

namespace Iptv::Ui {

class VideoPreviewWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit VideoPreviewWidget(QWidget *parent = nullptr);
    ~VideoPreviewWidget();

    void setData(uchar *data, int width, int height);
    void clearPage();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    uchar *m_data = nullptr;
    int m_width = 0;
    int m_height = 0;
};

} // namespace Iptv::Ui
