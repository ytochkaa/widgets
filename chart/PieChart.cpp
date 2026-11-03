#include "chart/PieChart.h"
#include <QtCharts/QPieSeries>
#include <QtCharts/QPieSlice>
#include <QtMath>

QT_CHARTS_USE_NAMESPACE

QChart* PieChart::buildGraf(const QVector<DataPoint>& points, const IChartStyle& style) const
{
    QVector<DataPoint> groups = groupPoints(points, 10);

    auto* series = new QPieSeries();
    for (int i = 0; i < groups.size(); ++i) {
        QPieSlice* slice = series->append(groups[i].time, qFabs(groups[i].value));
        slice->setColor(style.colorAt(i, groups.size()));
    }

    auto* chart = new QChart();
    chart->addSeries(series);
    chart->legend()->setAlignment(Qt::AlignRight);
    return chart;
}
