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
     * @return 时间长度(ms)。
     */
    qint64 viewLengthToTimeLength(int length);

    /**
     * 获取时间长度在当前状态下等同的视口像素宽度，精确到整数的估计值。
     * @param length 时间长度(ms)
     * @return 像素长度。
     */
    int timeLengthToViewLength(qint64 length);

    /**
     * 设置内容控件。
     * @param widget 内容控件。
     */
    void setContent(QWidget* widget);

    /**
     * 设置视口左边距，默认为 10，建议设置不小于10的数字。
     * @param padding 左边距
     */
    void setPaddingLeft(int padding);

    /**
     * 设置视口右边距，默认为 10, 建议设置不小于10的数字。
     * @param padding 右边距
     */
    void setPaddingRight(int padding);

    /**
     * 设置视口上边距，默认为 30, 建议设置不小于30的数字。
     * @param padding 上边距。
     */
    void setPaddingTop(int padding);

    /**
     * 设置视口下边距，默认为 20, 建议设置不小于20的数字。
     * @param padding 下边距。
     */
    void setPaddingBottom(int padding);

    /**
     * 获取当前视口左边距。
     * @return 左边距。
     */
    int paddingLeft() const;

    /**
     * 获取当前视口右边距
     * @return 右边距。
     */
    int paddingRight() const;

    /**
     * 获取当前视口上边距。
     * @return 上边距。
     */
    int paddingTop() const;

    /**
     * 获取当前视口下边距。
     * @return 下边距。
     */
    int paddingBottom() const;

    /**
     * 获取当前视口几何，位置及尺寸。
     * @return 当前视口几何。
     */
    QRect viewGeometry() const;

    /**
     * 获取当前遮罩层对象。
     * @return 遮罩层对象。
     */
    QWidget* cover() const;

    /**
     * 设置当前时间（对应标尺）。
     * @param dateTime 当前时间。
     */
    void setCurrentDateTime(const QDateTime &dateTime);

    /**
     * 获取当前时间。
     * @return 当前时间
     */
    QDateTime currentDateTime();
signals:
    /**
     * 内容区域尺寸变化信号，用户可连接此信号来更新内容区域的变化（如缩放）
     * @param width 宽度。
     * @param height 高度。
     */
    void contentSizeChanged(int width, int height);

    /**
     * 当前时间变化信号。
     * @param dateTime 最新的当前时间。
     */
    void currentDateTimeChanged(const QDateTime &dateTime);
protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
private:
    QTimeAxisPrivate *d;
};



#endif
