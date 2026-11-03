#ifndef PIECHART_H
#define PIECHART_H

#include "chart/IChart.h"

/**
 * @brief Круговая диаграмма: один сектор на группу точек.
 */
class PieChart : public IChart
{
public:
    /**
     * @brief Строит круговую диаграмму (точки сжимаются до 10 групп).
     *
     * Сектор не может быть отрицательным, поэтому берётся модуль значения.
     * @param points точки данных.
     * @param style  стиль, задающий цвета секторов.
     * @return новый график.
     */
    QtCharts::QChart* buildGraf(const QVector<DataPoint>& points, const IChartStyle& style) const override;
};

#endif
