#include "data/DataPoint.h"

QVector<DataPoint> groupPoints(const QVector<DataPoint>& points, int count)
{
    if (points.size() <= count) {
        return points;
    }

    QVector<DataPoint> groups;
    for (int g = 0; g < count; ++g) {
        int begin = points.size() * g / count;
        int end = points.size() * (g + 1) / count;

        double sum = 0;
        for (int i = begin; i < end; ++i) {
            sum += points[i].value;
        }
        groups.append({points[begin].time, sum / (end - begin)});
    }
    return groups;
}
