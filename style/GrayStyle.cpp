#include "style/GrayStyle.h"

QColor GrayStyle::colorAt(int i, int n) const
{
    if (n <= 1) {
        return QColor(60, 60, 60);
    }
    int gray = 40 + 170 * i / (n - 1);
    return QColor(gray, gray, gray);
}
