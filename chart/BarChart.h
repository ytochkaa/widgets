#ifndef BARCHART_H
#define BARCHART_H

#include "chart/IChart.h"

/**
 * @brief Столбчатая диаграмма: один столбик на группу точек.
 */
class BarChart : public IChart
{
public:
    /**
     * @brief Строит столбчатую диаграмму (точки сжимаются до 10 групп).
     * @param points точки данных.
     * @param style  стиль, задающий цвета столбиков.
     * @return новый график.
     */
    QtCharts::QChart* buildGraf(const QVector<DataPoint>& points, const IChartStyle& style) const override;
};

#endif
