#include <QApplication>
#include <locale>
#include "gui/AppCompositor.h"
#include "gui/MainWindow.h"
#include "ioc/IOCContainer.h"

int main(int argc, char* argv[])
{
    setlocale(LC_ALL, "");
    QApplication a(argc, argv);

    // Контейнер объявлен раньше окна, поэтому живёт дольше него
    IOCContainer container;
    std::unique_ptr<MainWindow> window = CreateMainWindow(container);
    window->show();
    return a.exec();
}
