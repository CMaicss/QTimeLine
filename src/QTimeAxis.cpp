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
        QDateTime pointerTime(QDate(pd->viewStart.date().year(), pd->viewStart.date().month(), pd->viewStart.date().day()),
            QTime(0, 0, 0));

        // 绘制视口边框
        painter.drawRect(pd->view->geometry());

        int lastTextX = 0;
        while (pointerTime < pd->viewEnd) {
            int x = p->timeInView(pointerTime);
            pointerTime = pointerTime.addDays(1);
            QString text;
            int lineLength = 7;
            if (pointerTime.date().day() != 1) {
                text = QString::number(pointerTime.date().day()) +"日";
            } else {
                text = QString::number(pointerTime.date().month()) + "月" + QString::number(pointerTime.date().day()) +"日";
                lineLength = 15;
            }
            if (x < 0) continue;
            int xpos = x + pd->view->x();
            painter.drawLine(xpos, pd->view->y(),
                xpos, pd->view->y() + lineLength);
            painter.drawLine(xpos, pd->view->y() + pd->view->height(),
                xpos, pd->view->y() + pd->view->height() - lineLength);
            int textWidth = painter.fontMetrics().boundingRect(text).width() + 20;
            // 防止绘制的文字相互遮盖
            if (xpos - textWidth / 2 > lastTextX) {
                painter.drawText(QRect(xpos - textWidth / 2, 0, textWidth, pd->paddingTop), Qt::AlignHCenter | Qt::AlignBottom, text);
                lastTextX = xpos + textWidth / 2;
            }
        }

    }

private:
    QTimeAxis *p;
    QTimeAxisPrivate *pd;
};

void QTimeAxisPrivate::updateView() {
    view->setGeometry(paddingRight, paddingTop,
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
    auto subTime = d->viewEnd.toMSecsSinceEpoch() - d->viewStart.toMSecsSinceEpoch();
    return d->viewStart.addMSecs(subTime * p);
}

qint64 QTimeAxis::viewLengthToTimeLength(int length) {
    auto subTime = d->viewEnd.toMSecsSinceEpoch() - d->viewStart.toMSecsSinceEpoch();
    auto subX = d->view->width();
    return subTime * length / subX;
}

void QTimeAxis::setContent(QWidget *widget) {
    if (d->content != nullptr) {
        d->content->deleteLater();
    }
    d->content = widget;
    d->content->setParent(d->view);
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
}

void QTimeAxis::mouseMoveEvent(QMouseEvent *event) {
    QWidget::mouseMoveEvent(event);
    int subX = event->position().x() - d->startDragX;
    int subTime = viewLengthToTimeLength(subX);
    auto vs = d->startDragTimeS.addMSecs(-subTime);
    auto ve = d->startDragTimeE.addMSecs(-subTime);
    if (vs < d->startTime) {
        setViewRange(d->startTime, d->startTime.addMSecs(viewLengthToTimeLength(d->view->width())));
        return;
    }
    if (ve > d->endTime) {
        setViewRange(d->endTime.addMSecs(-viewLengthToTimeLength(d->view->width())), d->endTime);
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
    setViewRange(startTime, endTime);
}
