#ifndef COLORSTYLE_H
#define COLORSTYLE_H

#include "style/IChartStyle.h"

/**
 * @brief Цветной стиль: элементы раскрашиваются по кругу из набора цветов.
 */
class ColorStyle : public IChartStyle
{
public:
    /**
     * @brief Возвращает цвет из палитры по номеру элемента.
     * @param i номер элемента.
     * @param n общее число элементов (не используется).
     * @return цвет элемента.
     */
    QColor colorAt(int i, int n) const override;
};

#endif
