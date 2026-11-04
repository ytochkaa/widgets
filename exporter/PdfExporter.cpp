#include "exporter/PdfExporter.h"
#include <QPainter>
#include <QPdfWriter>
#include <QWidget>

PdfExporter::PdfExporter(std::shared_ptr<ILogger> logger) : logger(logger) {}

bool PdfExporter::exportChart(QWidget* chartView, const QString& path) const
{
    QPdfWriter writer(path);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setPageOrientation(QPageLayout::Landscape);

    QPainter painter(&writer);
    if (!painter.isActive()) {
        logger->logError("Не удалось создать PDF " + path);
        return false;
    }
    chartView->render(&painter);
    painter.end();

    logger->logInfo("График сохранён в " + path);
    return true;
}

QString PdfExporter::fileFilter() const
{
    return "PDF (*.pdf)";
}

QString PdfExporter::defaultFileName() const
{
    return "chart.pdf";
}
