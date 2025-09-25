#include <iostream>
#include <QApplication>
#include "example/GanttChart.h"
int main(int argc, char *argv[]) {
    std::cout << "Hello, World!" << std::endl;

    QApplication a(argc, argv);
    auto *axis = new GanttChart();
    axis->resize(800, 500);
    axis->show();
    axis->setLineRange(QDateTime(QDate(2025, 4, 1), QTime(0, 0, 0)),
    QDateTime(QDate(2025, 6, 10), QTime(0, 0, 0)));
    axis->setViewRange(QDateTime(QDate(2025, 4, 1), QTime(0, 0, 0)),
        QDateTime(QDate(2025, 4, 20), QTime(0, 0, 0)));

    QVector<GanttChartItem *> items;
    {
        GanttChartItem *item = new GanttChartItem(axis);
        item->insertRange(TimeRange(QDateTime(QDate(2025, 4, 1), QTime(0, 0, 0)),
                                    QDateTime(QDate(2025, 4, 10), QTime(0, 0, 0))));
        item->setText("任务1");
        items.push_back(item);
    }{
        GanttChartItem *item = new GanttChartItem(axis);
        item->insertRange(TimeRange(QDateTime(QDate(2025, 4, 10), QTime(0, 0, 0)),
                                    QDateTime(QDate(2025, 4, 20), QTime(0, 0, 0))));
        item->setText("任务2");
        items.push_back(item);
    }{
        GanttChartItem *item = new GanttChartItem(axis);
        item->insertRange(TimeRange(QDateTime(QDate(2025, 4, 15), QTime(0, 0, 0)),
                                    QDateTime(QDate(2025, 4, 25), QTime(0, 0, 0))));
        item->setText("任务3");
        items.push_back(item);
    }

    axis->setItems(items);

    return QApplication::exec();
}