#ifndef ICHARTSTYLE_H
#define ICHARTSTYLE_H

#include <QColor>

/**
 * @brief Абстрактный интерфейс визуального стиля графика.
 */
class IChartStyle
{
public:
    virtual ~IChartStyle() = default;

    /**
     * @brief Возвращает цвет элемента графика (столбика, сектора, линии).
     * @param i номер элемента.
     * @param n общее число элементов.
     * @return цвет элемента.
     */
    virtual QColor colorAt(int i, int n) const = 0;
};

#endif
