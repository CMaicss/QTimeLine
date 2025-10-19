//
// Created by Maicss on 25-6-25.
//

#ifndef GANTTCHART_H
#define GANTTCHART_H

#include <QDateTime>
#include <QTreeWidget>

#include "../src/QTimeAxis.h"

/**
 * 时间区间，描述一个条目中的区间之一。
 */
class TimeRange {
public:
    /**
     * 构造函数，开始时间和结束时间必须制定。
     * @param from 区间开始时间。
     * @param to 区间结束时间，若为空则按当前时间。
     */
    explicit TimeRange(const QDateTime &from, const QDateTime &to={}) {
        this->m_from = from;
        this->m_to = to;
    }

    /**
     * 获取区间开始时间。
     * @return 区间开始时间。
     */
    QDateTime from() { return m_from;}

    /**
     * 获取区间结束时间。
     * @return 区间结束时间。
     */
    QDateTime to() { return m_to;}

    /**
     * 设置区间开始时间。
     * @param from 开始时间。
     */
    void setFrom(const QDateTime &from) { m_from = from; }

    /**
     * 设置区间结束时间。
     * @param to 结束时间。
     */
    void setTo(const QDateTime &to) { m_to = to; }

    /**
     * 设置区间色带内显示的文字。
     * @param text 文字。
     */
    void setText(const QString& text) { m_text = text;}

    /**
     * 获取区间色带内显示的文字。
     * @return 文字。
     */
    QString text() { return m_text;}

    /**
     * 设置区间色带的背景颜色，默认蓝色。
     * @param color 背景颜色。
     */
    void setBackgroundColor(const QColor& color) { m_backgroundColor = color;}

    /**
     * 获取区间色带的背景颜色。
     * @return 背景颜色。
     */
    QColor backgroundColor() const { return m_backgroundColor;}

    /**
     * 设置区间内的文本颜色，默认白色。
     * @param color 文本颜色。
     */
    void setTextColor(const QColor& color) { m_textColor = color;}

    /**
     * 获取区间内的文本颜色。
     * @return 文本颜色。
     */
    QColor textColor() const { return m_textColor;}
private:
    QDateTime m_from;
    QDateTime m_to;
    QString m_text;
    QColor m_backgroundColor = QColor(0x3574F0);
    QColor m_textColor = Qt::white;
};
class GanttChart;

/**
 * 甘特图条目。
 */
class GanttChartItem : public QWidget {
    Q_OBJECT
    friend class GanttChart;
public:
    explicit GanttChartItem(GanttChart* ganttChart);
    explicit GanttChartItem(GanttChartItem* item);
    virtual ~GanttChartItem();

    /**
     * 插入区间（任务）
     * @param range 要插入的时间区间
     */
    void insertRange(const TimeRange &range) {
        m_ranges.push_back(range);
    }

    /**
     * 获取全部区间（任务）
     * @return 全部区间列表
     */
    QList<TimeRange>& ranges() { return m_ranges;}

    /**
     * 设置此行的名称，显示在左侧列表。
     * @param name 名称。
     */
    void setName(const QString& name) { m_name = name;}

    /**
     * 获取此行的名称。
     * @return 此行的名称
     */
    QString name() { return m_name;}

    /**
     * 获取子节点列表。
     * @return 获取子节点列表
     */
    QVector<GanttChartItem*> childList();
protected:
    void paintEvent(QPaintEvent *event) override;

    void setItem(QTreeWidgetItem *item) {
        m_item = item;
    }

    QTreeWidgetItem* item() const { return m_item;}

    void setExpanded(bool expanded) { m_itemExpanded = expanded;}
    bool expanded() const { return m_itemExpanded;}
private:
    void addChildItem(GanttChartItem *item);

    QVector<TimeRange> m_ranges;
    GanttChart* m_ganttChart{};
    QString m_name;
    QVector<GanttChartItem*> m_child;
    QTreeWidgetItem* m_item;
    bool m_itemExpanded{true};
};

/**
 * 甘特图控件，基于时间轴控件派生而来。
 */
class GanttChart : public QTimeAxis {
    Q_OBJECT
public:
    explicit GanttChart(QWidget *parent = 0);

    /**
     * 设置图中所有的条目。
     * @param items 图中所有的行。
     */
    void setItems(QVector<GanttChartItem*> items);

    /**
     * 获取目前图中所有条目。
     * @return 目前图中所有的行。
     */
    QVector<GanttChartItem*> items() { return m_items;}

    int maxScreen();
protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void updateLayout();
private:
    QWidget *m_widget;
    QVector<GanttChartItem *> m_items;
    int m_offset{0};
    QDateTime m_currentDateTime;
    QTreeWidget* m_treeWidget;
};



#endif //GANTTCHART_H
