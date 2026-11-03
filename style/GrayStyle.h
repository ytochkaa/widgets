#ifndef GRAYSTYLE_H
#define GRAYSTYLE_H

#include "style/IChartStyle.h"

/**
 * @brief Чёрно-белый стиль: элементы раскрашиваются оттенками серого от тёмного к светлому.
 */
class GrayStyle : public IChartStyle
{
public:
    /**
     * @brief Возвращает оттенок серого по номеру элемента.
     * @param i номер элемента.
     * @param n общее число элементов.
     * @return серый цвет элемента.
     */
    QColor colorAt(int i, int n) const override;
};

#endif
