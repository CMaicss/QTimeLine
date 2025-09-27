#include "QTimeAxis.h"
#include <QDateTime>
#include <QLabel>
#include <QPainter>
#include <QWheelEvent>
#include <QPropertyAnimation>

class CoverWidget;
class QTimeAxisPrivate {
public:
    explicit QTimeAxisPrivate(QTimeAxis* parent) : parent(parent) {}

    QDateTime startTime;
    QDateTime endTime;

    QDateTime viewStart;
    QDateTime viewEnd;

    QDateTime currentDateTime;

    QWidget* view;
    QWidget* content = nullptr;

    void updateView();
    void updateContent();

    int paddingTop = 30;
    int paddingRight = 10;
    int paddingBottom = 20;
    int paddingLeft = 10;

    bool inDrag = false;
    int startDragX = 0;
    QDateTime startDragTimeS; ///< 开始拖拽时的视口开始时间
    QDateTime startDragTimeE; ///< 开始拖拽时的视口结束时间

    CoverWidget *cover;

private:
    QTimeAxis* parent;

};

class CoverWidget : public QWidget {
public:
    CoverWidget(QTimeAxis *parent, QTimeAxisPrivate* parent_d) : QWidget(parent), p(parent), pd(parent_d) {
        setAttribute(Qt::WA_TransparentForMouseEvents);
    }
protected:
    void paintEvent(QPaintEvent *event) override {
        QWidget::paintEvent(event);
        QPainter painter(this);

        QDateTime pointerTime;
        // 绘制视口边框
        painter.drawRect(pd->view->geometry());
        int type = 4;
        int minLen = p->timeLengthToViewLength(60);
        int hourLen = p->timeLengthToViewLength(60 * 60);
        int dayLen = p->timeLengthToViewLength(60 * 60 * 24);
        int monthLen = p->timeLengthToViewLength(60 * 60 * 24 * 30);
        if (monthLen < 365) {
            // 按月刻度
            type = 1;
        } else if (dayLen < 365) {
            // 按日刻度
            type = 2;
        } else if (hourLen < 365) {
            // 按时刻度
            type = 3;
        } else if (minLen < 310) {
            // 按分刻度
            type = 4;
        }

        QString headText;
        int headTextWidth = 0;
        if (type == 1) {
            // 按月刻度
            headText = pd->viewStart.toString("yyyy年");
            headTextWidth = painter.fontMetrics().boundingRect(headText).width();
            painter.drawText(QRect(pd->view->x(), 0, headTextWidth, pd->paddingTop), Qt::AlignHCenter | Qt::AlignBottom, headText);
            pointerTime = QDateTime(QDate(pd->startTime.date().year(), pd->startTime.date().month(), 0),
            QTime(0, 0, 0));
        } else if (type == 2) {
            // 按日刻度
            headText = pd->viewStart.toString("yyyy年MM月");
            headTextWidth = painter.fontMetrics().boundingRect(headText).width();
            painter.drawText(QRect(pd->view->x(), 0, headTextWidth, pd->paddingTop), Qt::AlignHCenter | Qt::AlignBottom, headText);
            pointerTime = QDateTime(QDate(pd->startTime.date().year(), pd->startTime.date().month(), pd->startTime.date().day()),
            QTime(0, 0, 0));
        } else if (type == 3) {
            // 按时刻度
            headText = pd->viewStart.toString("yyyy年MM月dd日");
            headTextWidth = painter.fontMetrics().boundingRect(headText).width();
            painter.drawText(QRect(pd->view->x(), 0, headTextWidth, pd->paddingTop), Qt::AlignHCenter | Qt::AlignBottom, headText);
            pointerTime = QDateTime(QDate(pd->startTime.date().year(), pd->startTime.date().month(), pd->startTime.date().day()),
            QTime(pd->startTime.time().hour(), 0, 0));
        } else if (type == 4) {
            // 按分刻度
            headText = pd->viewStart.toString("yyyy年MM月dd日hh时");
            headTextWidth = painter.fontMetrics().boundingRect(headText).width();
            painter.drawText(QRect(pd->view->x(), 0, headTextWidth, pd->paddingTop), Qt::AlignHCenter | Qt::AlignBottom, headText);
            pointerTime = QDateTime(QDate(pd->startTime.date().year(), pd->startTime.date().month(), pd->startTime.date().day()),
            QTime(pd->startTime.time().hour(), pd->startTime.time().minute(), 0));
        }

        // LiuFC start 20250925: 注释此代码可使拖拽时，时间轴时间文字不闪烁，但会导致性能问题
        pointerTime = QDateTime(QDate(pd->viewStart.date().year(), pd->viewStart.date().month(), pd->viewStart.date().day()),
            QTime(0, 0, 0));
        // LiuFC end 20250925: 注释此代码可使拖拽时，时间轴时间文字不闪烁，但会导致性能问题
        int lastTextX = p->timeInView(pointerTime) + pd->view->x();
        while (pointerTime < pd->viewEnd) {
            QString text;
            int lineLength = 7;
            if (type == 1) {
                pointerTime = pointerTime.addMonths(1);
                if (pointerTime.date().month() != 1) {
                    text = QString::number(pointerTime.date().month()) +"月";
                } else {
                    text = QString::number(pointerTime.date().year()) + "年1月";
                    lineLength = 15;
                }
            } else if (type == 2) {
                pointerTime = pointerTime.addDays(1);
                if (pointerTime.date().day() != 1) {
                    text = QString::number(pointerTime.date().day()) +"日";
                } else {
                    text = QString::number(pointerTime.date().month()) + "月1日";
                    lineLength = 15;
                }
            } else if (type == 3) {
                pointerTime = pointerTime.addSecs(60 * 60);
                if (pointerTime.time().hour() != 0) {
                    text = QString::number(pointerTime.time().hour()) +"时";
                } else {
                    text = QString::number(pointerTime.date().day()) + "日0时";
                    lineLength = 15;
                }
            } else if (type == 4) {
                pointerTime = pointerTime.addSecs(60);
                if (pointerTime.time().minute() != 0) {
                    text = QString::number(pointerTime.time().minute()) +"分";
                } else {
                    text = QString::number(pointerTime.time().hour()) + "时整";
                    lineLength = 15;
                }
            }
            int x = p->timeInView(pointerTime);
            int xpos = x + pd->view->x();
            if (x > 0 && x < pd->view->width()) {
                painter.drawLine(xpos, pd->view->y(),
                    xpos, pd->view->y() + lineLength);
                painter.drawLine(xpos, pd->view->y() + pd->view->height(),
                    xpos, pd->view->y() + pd->view->height() - lineLength);
            }
            int textWidth = painter.fontMetrics().boundingRect(text).width() + 20;
            // int textWidth
            // 防止绘制的文字相互遮盖
            if (xpos - textWidth / 2 > lastTextX) {
                QRect textRect(xpos - textWidth / 2, 0, textWidth, pd->paddingTop);
                lastTextX = xpos + textWidth / 2;
                if (textRect.x() < headTextWidth + pd->view->x()) {
                    continue;
                }
                painter.drawText(textRect, Qt::AlignHCenter | Qt::AlignBottom, text);
            }
        }


        int x = p->timeInView(p->currentDateTime());
        if (x > 0 && x < p->viewGeometry().width()) {
            int paintX = x + p->paddingLeft();
            QPen pen;
            pen.setColor(QColor(0x3574F0));
            pen.setWidth(2);
            painter.setPen(pen);
            painter.drawLine(paintX, p->paddingTop() + 1, paintX, p->viewGeometry().height() + p->paddingTop());
        }

    }

private:
    QTimeAxis *p;
    QTimeAxisPrivate *pd;
};

void QTimeAxisPrivate::updateView() {
    view->setGeometry(paddingLeft, paddingTop,
        parent->width() - paddingRight - paddingLeft,
        parent->height() - paddingTop - paddingBottom);
    content->setFixedHeight(view->height());
}

void QTimeAxisPrivate::updateContent() {
    qint64 lineDur = startTime.toMSecsSinceEpoch() - endTime.toMSecsSinceEpoch();
    qint64 viewDur = viewStart.toMSecsSinceEpoch() - viewEnd.toMSecsSinceEpoch();

    auto contentWidth = (static_cast<double>(lineDur) / viewDur) * view->width();
    content->setFixedWidth(static_cast<int>(contentWidth));

    auto xOffsetTime = viewStart.toMSecsSinceEpoch() - startTime.toMSecsSinceEpoch();
    auto xOffset = (static_cast<double>(xOffsetTime) / lineDur) * contentWidth;
    content->move(static_cast<int>(xOffset), 0);
}

QTimeAxis::QTimeAxis(QWidget *parent)
    : d(new QTimeAxisPrivate(this)) {
    d->view = new QWidget(this);
    setContent(new QLabel("", d->view));
    d->startTime = QDateTime::currentDateTime();
    d->endTime = QDateTime::currentDateTime().addMonths(2);
    d->viewStart = QDateTime::currentDateTime().addSecs(60 * 60);
    d->viewEnd = QDateTime::currentDateTime().addMonths(1);
    d->cover = new CoverWidget(this, d);
    d->cover->raise();
    d->cover->setGeometry(this->rect());
}


QTimeAxis::~QTimeAxis() {
    delete d;
}

void QTimeAxis::setLineRange(const QDateTime &startTime, const QDateTime &endTime) {
    d->startTime = startTime;
    d->endTime = endTime;
}

void QTimeAxis::setViewRange(const QDateTime &startTime, const QDateTime &endTime) {
    d->viewStart = startTime;
    d->viewEnd = endTime;
    d->updateContent();
    emit contentSizeChanged(d->content->width(), d->content->height());
    update();
}

QDateTime QTimeAxis::startTime() const {
    return d->startTime;
}

QDateTime QTimeAxis::endTime() const {
    return d->endTime;
}

QDateTime QTimeAxis::viewStart() const {
    return d->viewStart;
}

QDateTime QTimeAxis::viewEnd() const {
    return d->viewEnd;
}

int QTimeAxis::timeInView(const QDateTime &time) {
    const auto len = d->viewEnd.toMSecsSinceEpoch() - d->viewStart.toMSecsSinceEpoch();
    const auto target = time.toMSecsSinceEpoch() - d->viewStart.toMSecsSinceEpoch();
    auto p = static_cast<double>(target) / len;
    return d->view->width() * p;
}

int QTimeAxis::timeInContent(const QDateTime &time) {
    const auto subTime = d->endTime.toMSecsSinceEpoch() - d->startTime.toMSecsSinceEpoch();
    const auto targetTime = time.toMSecsSinceEpoch() - d->startTime.toMSecsSinceEpoch();
    auto p = static_cast<double>(targetTime) / subTime;
    return d->content->width() * p;
}

QDateTime QTimeAxis::viewToTime(int x) {
    auto p = static_cast<double>(x) / static_cast<double>(d->view->width());
    auto subTime = d->viewEnd.toSecsSinceEpoch() - d->viewStart.toSecsSinceEpoch();
    return d->viewStart.addSecs(subTime * p);
}

qint64 QTimeAxis::viewLengthToTimeLength(int length) {
    auto subTime = d->viewEnd.toSecsSinceEpoch() - d->viewStart.toSecsSinceEpoch();
    auto subX = d->view->width();
    return subTime * length / subX;
}

int QTimeAxis::timeLengthToViewLength(qint64 length) {
    auto subTime = d->viewEnd.toSecsSinceEpoch() - d->viewStart.toSecsSinceEpoch();
    auto subX = d->view->width();
    return int(double(length)  / subTime * subX);
}

void QTimeAxis::setContent(QWidget *widget) {
    if (d->content != nullptr) {
        d->content->deleteLater();
    }
    d->content = widget;
    d->content->setParent(d->view);
}

void QTimeAxis::setPaddingLeft(int padding) {
    d->paddingLeft = padding;
    d->updateView();
    d->updateContent();
}

void QTimeAxis::setPaddingRight(int padding) {
    d->paddingRight = padding;
    d->updateView();
    d->updateContent();
}

void QTimeAxis::setPaddingTop(int padding) {
    d->paddingTop = padding;
    d->updateView();
    d->updateContent();
}

void QTimeAxis::setPaddingBottom(int padding) {
    d->paddingBottom = padding;
    d->updateView();
    d->updateContent();
}

int QTimeAxis::paddingLeft() const {
    return d->paddingLeft;
}

int QTimeAxis::paddingRight() const {
    return d->paddingRight;
}

int QTimeAxis::paddingTop() const {
    return d->paddingTop;
}

int QTimeAxis::paddingBottom() const {
    return d->paddingBottom;
}

QRect QTimeAxis::viewGeometry() const {
    return d->view->geometry();
}

QWidget * QTimeAxis::cover() const {
    return d->cover;
}

void QTimeAxis::setCurrentDateTime(const QDateTime &dateTime) {
    d->currentDateTime = dateTime;
    emit currentDateTimeChanged(dateTime);
    update();
}

QDateTime QTimeAxis::currentDateTime() {
    return d->currentDateTime;
}

void QTimeAxis::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
}

void QTimeAxis::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    d->updateView();
    d->updateContent();
    d->cover->setGeometry(this->rect());
    emit contentSizeChanged(d->content->width(), d->content->height());
}

void QTimeAxis::mousePressEvent(QMouseEvent *event) {
    QWidget::mousePressEvent(event);
    d->inDrag = true;
    d->startDragX = event->position().x();
    d->startDragTimeS = d->viewStart;
    d->startDragTimeE = d->viewEnd;
    int xInView = event->position().x() - paddingLeft();
    auto time = viewToTime(xInView);
    setCurrentDateTime(time);
}

void QTimeAxis::mouseMoveEvent(QMouseEvent *event) {
    QWidget::mouseMoveEvent(event);
    int subX = event->position().x() - d->startDragX;
    int subTime = viewLengthToTimeLength(subX);
    auto vs = d->startDragTimeS.addSecs(-subTime);
    auto ve = d->startDragTimeE.addSecs(-subTime);
    if (vs < d->startTime) {
        setViewRange(d->startTime, d->startTime.addSecs(viewLengthToTimeLength(d->view->width())));
        return;
    }
    if (ve > d->endTime) {
        setViewRange(d->endTime.addSecs(-viewLengthToTimeLength(d->view->width())), d->endTime);
        return;
    }
    setViewRange(vs, ve);

}

void QTimeAxis::mouseReleaseEvent(QMouseEvent *event) {
    QWidget::mouseReleaseEvent(event);
    d->inDrag = false;
}

void QTimeAxis::wheelEvent(QWheelEvent *event) {
    QWidget::wheelEvent(event);
    if (event->position().y() < d->view->y()) {

        qint64 subTime = d->viewEnd.toMSecsSinceEpoch() - d->viewStart.toMSecsSinceEpoch();
        int offset = subTime * event->angleDelta().y() * 0.001f;
        auto x = event->position().x() - d->view->x();
        int offsetRight = (double(x) / d->view->width()) * offset;
        int offsetLeft = (1.0 - (double(x) / d->view->width())) * offset;
        // 防止因缩放超出非法区间
        auto startTime = d->viewStart.addMSecs(-offsetRight);
        if (startTime <= d->startTime) {
            startTime = d->startTime;
        }
        auto endTime = d->viewEnd.addMSecs(offsetLeft);
        if (endTime >= d->endTime) {
            endTime = d->endTime;
        }

        qint64 lineDur = d->startTime.toMSecsSinceEpoch() - d->endTime.toMSecsSinceEpoch();
        qint64 viewDur = startTime.toMSecsSinceEpoch() - endTime.toMSecsSinceEpoch();
        auto contentWidth = (static_cast<double>(lineDur) / viewDur) * d->view->width();
        if (contentWidth > 16777215) {
            return;
        }
        setViewRange(startTime, endTime);
    }
}
