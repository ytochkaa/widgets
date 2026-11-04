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
    // Один объект на всё приложение
    container.RegisterInstance<ILogger, ConsoleLogger>();
    container.RegisterInstance<ColorStyle, ColorStyle>();
    container.RegisterInstance<GrayStyle, GrayStyle>();

    // Новый объект при каждом запросе; зависимость ILogger подставляет контейнер
    container.RegisterFactory<SqliteReader, SqliteReader, ILogger>();
    container.RegisterFactory<JsonReader, JsonReader, ILogger>();
    container.RegisterFactory<BarChart, BarChart>();
    container.RegisterFactory<PieChart, PieChart>();
    container.RegisterFactory<LineChart, LineChart>();
    container.RegisterFactory<IChartExporter, PdfExporter, ILogger>();

    // Контейнер хранит одну реализацию на тип, поэтому варианты выбора
    // собраны в словари «ключ → получение объекта из контейнера»
    ReaderFactories readers = {
        {"sqlite", [&container] { return container.GetObject<SqliteReader>(); }},
        {"json", [&container] { return container.GetObject<JsonReader>(); }},
    };

    ChartFactories charts = {
        {"BarChart", [&container] { return container.GetObject<BarChart>(); }},
        {"PieChart", [&container] { return container.GetObject<PieChart>(); }},
        {"LineChart", [&container] { return container.GetObject<LineChart>(); }},
    };

    ObjectFactory<IChartStyle> colorStyle = [&container] { return container.GetObject<ColorStyle>(); };
    ObjectFactory<IChartStyle> grayStyle = [&container] { return container.GetObject<GrayStyle>(); };

    return std::make_unique<MainWindow>(container.GetObject<ILogger>(), readers, charts, colorStyle, grayStyle,
                                        container.GetObject<IChartExporter>(), parent);
}
