#ifndef DATAPOINT_H
#define DATAPOINT_H

#include <QString>
#include <QVector>

/**
 * @brief Одна точка данных: дата и значение.
 */
struct DataPoint
{
    QString time;
    double value;
};

/**
 * @brief Сжимает список точек до заданного числа групп.
 *
 * Точки делятся на count подряд идущих групп, значение группы — среднее
 * значений её точек, дата группы — дата первой точки. Нужна, чтобы
 * на графике не было десятков тысяч столбиков или секторов.
 * @param points исходные точки.
 * @param count  желаемое число групп.
 * @return сгруппированные точки; если точек не больше count — исходный список.
 */
QVector<DataPoint> groupPoints(const QVector<DataPoint>& points, int count);

#endif
