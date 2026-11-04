#include "gui/AppCompositor.h"
#include "chart/BarChart.h"
#include "chart/LineChart.h"
#include "chart/PieChart.h"
#include "data/JsonReader.h"
#include "data/SqliteReader.h"
#include "exporter/PdfExporter.h"
#include "gui/MainWindow.h"
#include "ioc/IOCContainer.h"
#include "logger/ConsoleLogger.h"
#include "style/ColorStyle.h"
#include "style/GrayStyle.h"

std::unique_ptr<MainWindow> CreateMainWindow(IOCContainer& container, QWidget* parent)
{
    // В контейнере — классы с зависимостями: ILogger им подставляет контейнер
    container.RegisterInstance<ILogger, ConsoleLogger>();                 // один объект на всё приложение
    container.RegisterFactory<SqliteReader, SqliteReader, ILogger>();     // новый объект при каждом запросе
    container.RegisterFactory<JsonReader, JsonReader, ILogger>();
    container.RegisterFactory<IChartExporter, PdfExporter, ILogger>();

    // Контейнер хранит одну реализацию на тип, поэтому варианты выбора
    // собраны в словари «ключ → получение объекта»
    ReaderFactories readers = {
        {"sqlite", [&container] { return container.GetObject<SqliteReader>(); }},
        {"json", [&container] { return container.GetObject<JsonReader>(); }},
    };

    // Графики и стили не имеют зависимостей, поэтому создаются напрямую, без контейнера
    ChartFactories charts = {
        {"BarChart", [] { return std::make_shared<BarChart>(); }},
        {"PieChart", [] { return std::make_shared<PieChart>(); }},
        {"LineChart", [] { return std::make_shared<LineChart>(); }},
    };

    ObjectFactory<IChartStyle> colorStyle = [] { return std::make_shared<ColorStyle>(); };
    ObjectFactory<IChartStyle> grayStyle = [] { return std::make_shared<GrayStyle>(); };

    return std::make_unique<MainWindow>(container.GetObject<ILogger>(), readers, charts, colorStyle, grayStyle,
                                        container.GetObject<IChartExporter>(), parent);
}
