#ifndef APPCOMPOSITOR_H
#define APPCOMPOSITOR_H

#include <memory>

class IOCContainer;
class MainWindow;
class QWidget;

/**
 * @brief Точка композиции приложения.
 *
 * Регистрирует в IoC-контейнере классы с зависимостями (логгер, чтение данных,
 * сохранение в PDF), создаёт графики и стили, собирает словари выбора
 * и создаёт главное окно. Только здесь известны конкретные классы.
 * @param container контейнер; должен жить дольше окна.
 * @param parent    родительский виджет Qt.
 * @return главное окно.
 */
std::unique_ptr<MainWindow> CreateMainWindow(IOCContainer& container, QWidget* parent = nullptr);

#endif
