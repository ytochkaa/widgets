#include "chart/LineChart.h"
#include <QtCharts/QLineSeries>
#include <QtCharts/QValueAxis>

QT_CHARTS_USE_NAMESPACE

QChart* LineChart::buildGraf(const QVector<DataPoint>& points, const IChartStyle& style) const
{
    QVector<DataPoint> groups = groupPoints(points, 1000);

    auto* series = new QLineSeries();
    series->setName(points.first().time + " — " + points.last().time);
    for (int i = 0; i < groups.size(); ++i) {
        series->append(i, groups[i].value);
    }

    QPen pen(style.colorAt(0, 1));
    pen.setWidth(2);
    series->setPen(pen);

    auto* chart = new QChart();
    chart->addSeries(series);
    chart->legend()->setAlignment(Qt::AlignTop);

    auto* axisX = new QValueAxis();
    axisX->setLabelsVisible(false);
    axisX->setTitleText("Время");
    chart->addAxis(axisX, Qt::AlignBottom);
    series->attachAxis(axisX);

    auto* axisY = new QValueAxis();
    chart->addAxis(axisY, Qt::AlignLeft);
    series->attachAxis(axisY);

    return chart;
}
