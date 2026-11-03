#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCheckBox>
#include <QComboBox>
#include <QFileSystemModel>
#include <QItemSelection>
#include <QMainWindow>
#include <QTableView>
#include <QtCharts/QChartView>
#include <memory>
#include "data/DataPoint.h"
#include "ioc/IOCContainer.h"
#include "logger/ILogger.h"

/**
 * @brief Главное окно: список файлов слева, график и настройки справа.
 *
 * Окно знает только интерфейсы IDataReader, IChart и IChartStyle.
 * Конкретные реализации регистрируются в IoC-контейнере при действиях пользователя
 * и получаются из него при построении графика.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор: создаёт интерфейс окна.
     * @param container IoC-контейнер с зарегистрированными зависимостями.
     * @param parent    родительский виджет Qt.
     */
    explicit MainWindow(IOCContainer& container, QWidget* parent = nullptr);

private slots:
    /**
     * @brief Открывает диалог выбора папки и показывает её файлы.
     */
    void openFolder();

    /**
     * @brief Загружает данные из выбранного файла и строит график.
     *
     * По расширению файла регистрирует в контейнере нужный IDataReader.
     * @param selected   выделенные элементы.
     * @param deselected элементы, с которых снято выделение (не используется).
     */
    void selectFile(const QItemSelection& selected, const QItemSelection& deselected);

    /**
     * @brief Регистрирует в контейнере выбранный тип графика и перерисовывает график.
     * @param index номер пункта в списке типов.
     */
    void changeChartType(int index);

    /**
     * @brief Регистрирует в контейнере цветной или чёрно-белый стиль и перерисовывает график.
     * @param gray true — чёрно-белый стиль.
     */
    void changeStyle(bool gray);

    /**
     * @brief Сохраняет текущий график в PDF в выбранное пользователем место.
     */
    void printChart();

private:
    /**
     * @brief Строит график по текущим данным с текущими типом и стилем.
     */
    void redraw();

    IOCContainer& container;
    std::shared_ptr<ILogger> logger;
    QVector<DataPoint> points;

    QFileSystemModel* fileModel;
    QTableView* fileView;
    QComboBox* chartTypeBox;
    QCheckBox* grayCheck;
    QtCharts::QChartView* chartView;
};

#endif
