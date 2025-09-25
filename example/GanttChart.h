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
    QDateTime from() { return m_from;}
    QDateTime to() { return m_to;}
    void setText(const QString& text) { m_text = text;}
    QString text() { return m_text;}
    void setBackgroundColor(const QColor& color) { m_backgroundColor = color;}
    QColor backgroundColor() const { return m_backgroundColor;}
    void setTextColor(const QColor& color) { m_textColor = color;}
    QColor textColor() const { return m_textColor;}
private:
    QDateTime m_from;
    QDateTime m_to;
    QString m_text;
    QColor m_backgroundColor = QColor(0x3574F0);
    QColor m_textColor = Qt::white;
};
class GanttChart;
class GanttChartItem : public QWidget {
    Q_OBJECT
public:
    GanttChartItem(GanttChart* ganttChart);
    void insertRange(TimeRange range) {
        m_ranges.push_back(range);
    }
    QList<TimeRange> ranges() { return m_ranges;}
    void setName(const QString& name) { m_name = name;}
    QString name() { return m_name;}
protected:
    void paintEvent(QPaintEvent *event);
private:
    QVector<TimeRange> m_ranges;
    GanttChart* m_ganttChart;
    QString m_name;
};

class GanttChart : public QTimeAxis {
    Q_OBJECT
public:
    explicit GanttChart(QWidget *parent = 0);

    void setItems(QVector<GanttChartItem*> items);

protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;

    void updateLayout();
private:
    QWidget *m_widget;
    QVector<GanttChartItem *> m_items;
    int m_offset{0};
};



#endif //GANTTCHART_H
