#ifndef PDFEXPORTER_H
#define PDFEXPORTER_H

#include <memory>
#include "exporter/IChartExporter.h"
#include "logger/ILogger.h"

/**
 * @brief Сохранение графика в PDF (страница A4, альбомная ориентация).
 */
class PdfExporter : public IChartExporter
{
public:
    /**
     * @brief Конструктор.
     * @param logger логгер для вывода сообщений.
     */
    explicit PdfExporter(std::shared_ptr<ILogger> logger);

    bool exportChart(QWidget* chartView, const QString& path) const override;
    QString fileFilter() const override;
    QString defaultFileName() const override;

private:
    std::shared_ptr<ILogger> logger;
};

#endif
