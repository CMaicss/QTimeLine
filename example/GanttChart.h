//
// Created by Maicss on 25-6-25.
//

#ifndef GANTTCHART_H
#define GANTTCHART_H

#include <QDateTime>

#include "../src/QTimeAxis.h"

class TimeRange {
public:
    TimeRange(QDateTime from, QDateTime to) {
        this->m_from = from;
        this->m_to = to;
    }
    QDateTime from() { return m_from;};
    QDateTime to() { return m_to;};
private:
    QDateTime m_from;
    QDateTime m_to;
};
class GanttChart;
class GanttChartItem : public QWidget {
    Q_OBJECT
public:
    GanttChartItem(GanttChart* ganttChart);
    void insertRange(TimeRange range) {
        m_ranges.push_back(range);
    }
    void setText(const QString& text) {
        m_text = text;
    }
    QList<TimeRange> ranges() { return m_ranges;}
protected:
    void paintEvent(QPaintEvent *event);
private:
    QVector<TimeRange> m_ranges;
    GanttChart* m_ganttChart;
    QString m_text;
};

class GanttChart : public QTimeAxis {
    Q_OBJECT
public:
    explicit GanttChart(QWidget *parent = 0);

    void setItems(QVector<GanttChartItem*> items);
private:
    QWidget *m_widget;
    QVector<GanttChartItem *> m_items;
};



#endif //GANTTCHART_H
