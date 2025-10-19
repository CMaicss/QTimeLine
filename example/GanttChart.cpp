//
// Created by Maicss on 25-6-25.
//
#include <QPainter>
#include <QPainterPath>
#include <QScrollBar>
#include "GanttChart.h"

#include <QWheelEvent>

GanttChartItem::GanttChartItem(GanttChart *ganttChart)
    : m_ganttChart(ganttChart){
}

GanttChartItem::GanttChartItem(GanttChartItem *item)
    : m_ganttChart(item->m_ganttChart) {
    item->addChildItem(this);
}

GanttChartItem::~GanttChartItem() {
    for (const auto item : m_child) {
        delete item;
    }
}

QVector<GanttChartItem *> GanttChartItem::childList() {
    return m_child;
}

void GanttChartItem::paintEvent(QPaintEvent *event) {
    QWidget::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    int padding = 6;
    for (auto range : ranges()) {
        int x1 = m_ganttChart->timeInContent(range.from());
        int x2 = m_ganttChart->timeInContent(range.to());
        if (range.to().isNull()) {
            x2 = m_ganttChart->timeInContent(m_ganttChart->currentDateTime());
        }
        if (x1 >= x2) {
            continue;
        }
        int viewStartX = m_ganttChart->timeInContent(m_ganttChart->viewStart());
        int viewEndX = m_ganttChart->timeInContent(m_ganttChart->viewEnd());
        painter.setPen(Qt::NoPen);
        painter.setBrush(range.backgroundColor());
        const int radius = 8;
        QPainterPath radiusRect;
        radiusRect.moveTo(x1 + radius, padding);
        radiusRect.arcTo(x1, padding, 2 * radius, 2 * radius, 90, 90);
        radiusRect.arcTo(x1, height() - padding - 2 * radius, 2 * radius, 2 * radius, 180, 90);
        // 判断若将结束时间设置为当前时间，则色带末尾不使用圆角
        if (range.to().isNull()) {
            radiusRect.lineTo(x2, height() - padding);
            radiusRect.lineTo(x2, padding);
        } else {
            radiusRect.arcTo(x2 - 2 * radius, height() - padding - 2 * radius, 2 * radius, 2 * radius, 270, 90);
            radiusRect.arcTo(x2 - 2 * radius, padding, 2 * radius, 2 * radius, 0, 90);
        }
        painter.drawPath(radiusRect);
        // painter.drawRoundedRect(x1, padding, x2 - x1, height() - padding * 2, 6, 6);
        QPen pen;
        pen.setColor(range.textColor());
        painter.setPen(pen);
        int textX1 = x1 > viewStartX ? x1 : viewStartX;
        int textX2 = x2 < viewEndX ? x2 : viewEndX;
        painter.drawText(textX1, padding, textX2 - textX1, height() - padding * 2, Qt::AlignHCenter | Qt::AlignVCenter, range.text());
    }
}

void GanttChartItem::addChildItem(GanttChartItem *item) {
    m_child.append(item);
}

GanttChart::GanttChart(QWidget *parent)
    : m_treeWidget(new QTreeWidget(this)){
    m_widget = new QWidget(this);
    setContent(m_widget);
    setPaddingLeft(100);

    connect(this, &QTimeAxis::contentSizeChanged, [=](int width, int height) {
       for (auto item : m_items) {
           item->setFixedWidth(width);
       }
    });

    m_treeWidget->setHeaderHidden(true);
    m_treeWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_treeWidget->setLineWidth(0);
    m_treeWidget->setBackgroundRole(QPalette::NoRole);

    connect(m_treeWidget->verticalScrollBar(), &QScrollBar::valueChanged, [=](int value) {
        m_treeWidget->verticalScrollBar()->blockSignals(true);
        m_offset = -maxScreen() * (double(m_treeWidget->verticalScrollBar()->value()) / m_treeWidget->verticalScrollBar()->maximum());
        updateLayout();
        update();
        m_treeWidget->verticalScrollBar()->blockSignals(false);
    });


    connect(m_treeWidget, &QTreeWidget::itemCollapsed, this, [=](QTreeWidgetItem *item) {
        for (const auto i : m_items) {
            if (i->item() == item) {
                for (auto child : i->childList()) {
                    child->setExpanded(false);
                }
            }
        }
        updateLayout();
    });

    connect(m_treeWidget, &QTreeWidget::itemExpanded, this, [=](QTreeWidgetItem *item) {
        for (const auto i : m_items) {
            if (i->item() == item) {
                for (auto child : i->childList()) {
                    child->setExpanded(true);
                }
            }
        }
        updateLayout();
    });
}

void GanttChart::setItems(QVector<GanttChartItem *> items) {
    m_items.swap(items);
    int index = 0;
    for (auto item : m_items) {
        auto treeItem = new QTreeWidgetItem(QStringList() << item->name());
        treeItem->setSizeHint(0, QSize(100, 42));
        m_treeWidget->insertTopLevelItem(index, treeItem);
        for (auto child : item->childList()) {
            auto childItem = new QTreeWidgetItem(QStringList() << child->name());
            childItem->setSizeHint(0, QSize(100, 42));
            treeItem->addChild(childItem);
            child->setVisible(true);
            child->setItem(treeItem);
        }
        item->setItem(treeItem);
        item->setVisible(true);
        index++;
    }
    updateLayout();
    update();
    m_treeWidget->expandAll();
}

int GanttChart::maxScreen() {
    int offsetMin = -m_items.size() * 42 + viewGeometry().height() - 30;
    return abs(offsetMin);
}

void GanttChart::paintEvent(QPaintEvent *event) {
    QTimeAxis::paintEvent(event);
}

void GanttChart::wheelEvent(QWheelEvent *event) {
    QTimeAxis::wheelEvent(event);
    int offsetMin = -m_items.size() * 42 + viewGeometry().height() - 30;
    if (viewGeometry().contains(event->position().toPoint())) {
        m_offset += event->angleDelta().y() / 5;
        if (m_offset > 0) m_offset = 0;
        int itemsHeight = m_items.size() * 42;
        if (m_offset < offsetMin) m_offset = offsetMin;
        if (itemsHeight < viewGeometry().height())
            m_offset = 0;
        updateLayout();
    }
    update();
    double percent = double(abs(m_offset)) / abs(offsetMin);
    m_treeWidget->verticalScrollBar()->setValue(m_treeWidget->verticalScrollBar()->maximum() * percent);
}

void GanttChart::updateLayout() {
    int index = 0;
    for (int i = 0; i < m_items.size(); i++) {
        GanttChartItem *item = m_items[i];
        item->setParent(m_widget);
        item->setFixedHeight(42);
        item->setFixedWidth(m_widget->width());
        item->move(0, index * item->height() + 15 + m_offset);
        item->setVisible(item->expanded());
        if (item->expanded()) {
            index++;
        }
    }
    m_treeWidget->setGeometry(0, viewGeometry().top() + 15,
        100, viewGeometry().height() - 30
    );

}
