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
    auto *item = new GanttChartItem(axis);
    TimeRange r1(QDateTime(QDate(2025, 4, 1), QTime(0, 0, 0)),
                                QDateTime(QDate(2025, 4, 10), QTime(0, 0, 0)));
    r1.setText("任务1_1");
    r1.setBackgroundColor(QColor(0x1BBC9B));
    item->insertRange(r1);
    TimeRange r2(QDateTime(QDate(2025, 5, 1), QTime(0, 0, 0)),
                                QDateTime(QDate(2025, 5, 10), QTime(0, 0, 0)));
    r2.setText("任务1_2");
    r2.setBackgroundColor(QColor(0x2DCC70));
    item->insertRange(r2);
    item->setName("实体1");
    items.push_back(item);

    auto *item2 = new GanttChartItem(item);
    TimeRange r1c(QDateTime(QDate(2025, 4, 10), QTime(0, 0, 0)),
                                QDateTime());
    r1c.setText("任务2_1");
    r1c.setBackgroundColor(QColor(0x3598DB));
    item2->insertRange(r1c);
    items.push_back(item2);
    item2->ranges()[0].setText("啦啦啦");
    item2->setName("实体2");
    for (int i = 3; i < 30; i++) {
        auto *item = new GanttChartItem(axis);
        TimeRange r1(QDateTime(QDate(2025, 4, 15), QTime(0, 0, 0)),
                                    QDateTime(QDate(2025, 4, 25), QTime(0, 0, 0)));
        r1.setText("任务" + QString::number(i) + "_1");
        r1.setBackgroundColor(QColor(0x9A59B5));
        item->insertRange(r1);
        items.push_back(item);
        item->setName("实体" + QString::number(i));
    }
    axis->setCurrentDateTime(QDateTime(QDate(2025, 5, 1), QTime(0, 0, 0)));

    axis->setItems(items);

    return QApplication::exec();
}