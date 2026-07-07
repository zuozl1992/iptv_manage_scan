#pragma once

#include <QOpenGLWidget>

namespace Iptv::Ui {

/**
 * @brief 视频预览控件
 *        使用OpenGL渲染视频帧数据，用于信号源检测时的画面预览
 */
class VideoPreviewWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    explicit VideoPreviewWidget(QWidget *parent = nullptr);
    ~VideoPreviewWidget();

    /** @brief 设置待渲染的帧数据
     *  @param data   BGRA像素数据指针
     *  @param width  图像宽度
     *  @param height 图像高度 */
    void setData(uchar *data, int width, int height);

    /** @brief 清空画面 */
    void clearPage();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    uchar *m_data = nullptr;    ///< BGRA像素数据
    int m_width = 0;            ///< 图像宽度
    int m_height = 0;           ///< 图像高度
};

} // namespace Iptv::Ui
