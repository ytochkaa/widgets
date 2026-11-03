#ifndef ICHART_H
#define ICHART_H

#include <QtCharts/QChart>
#include "data/DataPoint.h"
#include "style/IChartStyle.h"

/**
 * @brief Абстрактный интерфейс графика.
 *
 * Каждая реализация строит свой вид графика по точкам данных.
 */
class IChart
{
public:
    virtual ~IChart() = default;

    /**
     * @brief Строит график по точкам в заданном стиле.
     * @param points точки данных.
     * @param style  стиль, задающий цвета элементов.
     * @return новый график; владение передаётся вызывающему.
     */
    virtual QtCharts::QChart* buildGraf(const QVector<DataPoint>& points, const IChartStyle& style) const = 0;
};

#endif
