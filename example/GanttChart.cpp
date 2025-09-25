//
// Created by Maicss on 25-6-25.
//
#include <QPainter>
#include "GanttChart.h"

GanttChartItem::GanttChartItem(GanttChart *ganttChart)
    : m_ganttChart(ganttChart){
}

void GanttChartItem::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    QPainter painter(this);
    int padding = 6;
    for (auto range : ranges()) {
        int x1 = m_ganttChart->timeInContent(range.from());
        int x2 = m_ganttChart->timeInContent(range.to());
        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor("#3d84a8"));
        painter.drawRoundedRect(x1, padding, x2 - x1, height() - padding * 2, 6, 6);
        QPen pen;
        pen.setColor(Qt::white);
        painter.setPen(pen);
        painter.drawText(x1, padding, x2 - x1, height() - padding * 2, Qt::AlignHCenter | Qt::AlignVCenter, m_text);
    }
}

GanttChart::GanttChart(QWidget *parent) {
    m_widget = new QWidget(this);
    setContent(m_widget);

    connect(this, &QTimeAxis::contentSizeChanged, [=](int width, int height) {
       for (auto item : m_items) {
           item->setFixedWidth(width);
       }
    });
}

void GanttChart::setItems(QVector<GanttChartItem *> items) {
    for (int i = 0; i < items.size(); i++) {
        GanttChartItem *item = items[i];
        item->setParent(m_widget);
        item->setFixedHeight(32);
        item->setFixedWidth(m_widget->width());
        item->move(0, i * item->height() + 15);
        item->show();
        m_items.push_back(item);
    }

    update();
}
