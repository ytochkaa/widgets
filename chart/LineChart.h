#ifndef LINECHART_H
#define LINECHART_H

#include "chart/IChart.h"

/**
 * @brief Линейный график изменения значения во времени.
 */
class LineChart : public IChart
{
public:
    /**
     * @brief Строит линейный график (точки сжимаются до 1000 групп).
     * @param points точки данных.
     * @param style  стиль, задающий цвет линии.
     * @return новый график.
     */
    QtCharts::QChart* buildGraf(const QVector<DataPoint>& points, const IChartStyle& style) const override;
};

#endif
