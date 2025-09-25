//
// Created by Maicss on 25-6-25.
//
#include <QPainter>
#include "GanttChart.h"

#include <QWheelEvent>

GanttChartItem::GanttChartItem(GanttChart *ganttChart)
    : m_ganttChart(ganttChart){
}

void GanttChartItem::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    int padding = 6;
    for (auto range : ranges()) {
        int x1 = m_ganttChart->timeInContent(range.from());
        int x2 = m_ganttChart->timeInContent(range.to());
        int viewStartX = m_ganttChart->timeInContent(m_ganttChart->viewStart());
        int viewEndX = m_ganttChart->timeInContent(m_ganttChart->viewEnd());
        painter.setPen(Qt::NoPen);
        painter.setBrush(range.backgroundColor());
        painter.drawRoundedRect(x1, padding, x2 - x1, height() - padding * 2, 6, 6);
        QPen pen;
        pen.setColor(range.textColor());
        painter.setPen(pen);
        int textX1 = x1 > viewStartX ? x1 : viewStartX;
        int textX2 = x2 < viewEndX ? x2 : viewEndX;
        painter.drawText(textX1, padding, textX2 - textX1, height() - padding * 2, Qt::AlignHCenter | Qt::AlignVCenter, range.text());
    }
}

GanttChart::GanttChart(QWidget *parent) {
    m_widget = new QWidget(this);
    setContent(m_widget);
    setPaddingLeft(100);

    connect(this, &QTimeAxis::contentSizeChanged, [=](int width, int height) {
       for (auto item : m_items) {
           item->setFixedWidth(width);
       }
    });
}

void GanttChart::setItems(QVector<GanttChartItem *> items) {
    m_items.swap(items);
    updateLayout();
    update();
}

void GanttChart::paintEvent(QPaintEvent *event) {
    QTimeAxis::paintEvent(event);
    QPainter painter(this);
    for (int i = 0; i < m_items.size(); i++) {
        GanttChartItem *item = m_items[i];
        QRect nameRect(0, item->y() + paddingTop(), paddingLeft(), item->height());
        painter.drawText(nameRect, Qt::AlignCenter, item->name());
    }
}

void GanttChart::wheelEvent(QWheelEvent *event) {
    QTimeAxis::wheelEvent(event);
    if (viewGeometry().contains(event->position().toPoint())) {
        m_offset += event->angleDelta().y() / 5;
        if (m_offset > 0) m_offset = 0;
        int itemsHeight = m_items.size() * 42;
        if (itemsHeight + m_offset + 30 < viewGeometry().height())
            m_offset = -m_items.size() * 42 + viewGeometry().height() - 30;
        if (itemsHeight < viewGeometry().height())
            m_offset = 0;
        updateLayout();
    }
    update();
}

void GanttChart::updateLayout() {
    for (int i = 0; i < m_items.size(); i++) {
        GanttChartItem *item = m_items[i];
        item->setParent(m_widget);
        item->setFixedHeight(42);
        item->setFixedWidth(m_widget->width());
        item->move(0, i * item->height() + 15 + m_offset);
        item->show();
    }
}
