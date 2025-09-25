#ifndef QTIMEAXIS_H
#define QTIMEAXIS_H

#include <QWidget>


class QTimeAxisPrivate;
class QTimeAxis : public QWidget {
    Q_OBJECT

public:
    QTimeAxis(QWidget *parent = nullptr);
    ~QTimeAxis();

    /**
     * 设置整个时间轴的时间区间。
     * @param startTime 时间轴起始时间。
     * @param endTime 时间轴结束时间。
     */
    void setLineRange(const QDateTime& startTime, const QDateTime& endTime);

    /**
     * 设置当前视口的显示时间。
     * @param startTime 当前视口开始时间。
     * @param endTime 当前视口结束时间。
     */
    void setViewRange(const QDateTime& startTime, const QDateTime& endTime);

    /**
     * 获取整个时间轴的开始时间。
     * @return 开始时间。
     */
    QDateTime startTime() const;

    /**
     * 获取整个时间轴的结束时间。
     * @return 结束时间。
     */
    QDateTime endTime() const;

    /**
     * 获取当前视口的开始时间。
     * @return 开始时间。
     */
    QDateTime viewStart() const;

    /**
     * 获取当前视口的结束时间。
     * @return 结束时间。
     */
    QDateTime viewEnd() const;

    /**
     * 通过时间，获取其相对视口左上角的x轴位置。
     * @param time 时间。
     * @return x轴位置。
     */
    int timeInView(const QDateTime& time);

    /**
     * 通过时间，获取其相对内容组件左上角x轴的位置。
     * @param time 时间。
     * @return x轴位置。
     */
    int timeInContent(const QDateTime& time);

    /**
     * 通过视口的x坐标，返回其对应当前状态下的时间。
     * @param x x轴位置。
     * @return 时间。
     */
    QDateTime viewToTime(int x);

    /**
     * 获取视口像素宽度在当前状态下等同的时间长度。
     * @param length 视口的像素宽度。
     * @return 时间长度。
     */
    qint64 viewLengthToTimeLength(int length);

    /**
     * 设置内容控件。
     * @param widget 内容控件。
     */
    void setContent(QWidget* widget);

signals:
    void contentSizeChanged(int width, int height);
protected:
    void paintEvent(QPaintEvent *event);
    void resizeEvent(QResizeEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);
private:
    QTimeAxisPrivate *d;
};



#endif
