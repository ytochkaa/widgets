#include "style/ColorStyle.h"
#include <QVector>

QColor ColorStyle::colorAt(int i, int n) const
{
    Q_UNUSED(n);
    static const QVector<QColor> colors = {
        QColor(32, 159, 223),  QColor(153, 202, 83), QColor(246, 166, 37), QColor(109, 95, 213),
        QColor(191, 89, 62),   QColor(15, 63, 92),   QColor(66, 92, 41),   QColor(97, 66, 16),
        QColor(46, 42, 92),    QColor(91, 41, 33),
    };
    return colors[i % colors.size()];
}
