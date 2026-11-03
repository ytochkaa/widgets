#include "chart/BarChart.h"
#include <QtCharts/QBarCategoryAxis>
#include <QtCharts/QBarSeries>
#include <QtCharts/QBarSet>
#include <QtCharts/QValueAxis>

QT_CHARTS_USE_NAMESPACE

QChart* BarChart::buildGraf(const QVector<DataPoint>& points, const IChartStyle& style) const
{
    QVector<DataPoint> groups = groupPoints(points, 10);

    auto* series = new QBarSeries();
    for (int i = 0; i < groups.size(); ++i) {
        auto* set = new QBarSet(groups[i].time);
        *set << groups[i].value;
        set->setColor(style.colorAt(i, groups.size()));
        series->append(set);
    }

    auto* chart = new QChart();
    chart->addSeries(series);
    chart->legend()->setAlignment(Qt::AlignTop);

    auto* axisX = new QBarCategoryAxis();
    axisX->append(points.first().time + " — " + points.last().time);
    axisX->setTitleText("Период");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto* axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    return chart;
}
