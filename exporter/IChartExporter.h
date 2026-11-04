#ifndef ICHARTEXPORTER_H
#define ICHARTEXPORTER_H

#include <QString>

class QWidget;

/**
 * @brief Абстрактный интерфейс сохранения графика в файл.
 */
class IChartExporter
{
public:
    virtual ~IChartExporter() = default;

    /**
     * @brief Сохраняет изображение виджета с графиком в файл.
     * @param chartView виджет, на котором показан график.
     * @param path      путь к файлу.
     * @return true, если файл сохранён.
     */
    virtual bool exportChart(QWidget* chartView, const QString& path) const = 0;

    /**
     * @brief Фильтр для диалога сохранения, например "PDF (*.pdf)".
     */
    virtual QString fileFilter() const = 0;

    /**
     * @brief Имя файла, предлагаемое по умолчанию.
     */
    virtual QString defaultFileName() const = 0;
};

#endif
