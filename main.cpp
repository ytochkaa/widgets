#include <QApplication>
#include <locale>
#include "chart/BarChart.h"
#include "gui/MainWindow.h"
#include "ioc/IOCContainer.h"
#include "logger/ConsoleLogger.h"
#include "style/ColorStyle.h"

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "");
    QApplication a(argc, argv);

    // Регистрируем реализации по умолчанию: логгер, столбчатую диаграмму и цветной стиль
    IOCContainer container;
    container.RegisterInstance<ILogger, ConsoleLogger>();
    container.RegisterFactory<IChart, BarChart>();
    container.RegisterInstance<IChartStyle, ColorStyle>();

    MainWindow window(container);
    window.show();
    return a.exec();
}
