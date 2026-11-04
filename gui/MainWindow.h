#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QCheckBox>
#include <QComboBox>
#include <QFileSystemModel>
#include <QItemSelection>
#include <QMainWindow>
#include <QTableView>
#include <QtCharts/QChartView>
#include <functional>
#include <map>
#include <memory>
#include <utility>
#include <vector>
#include "chart/IChart.h"
#include "data/DataPoint.h"
#include "data/IDataReader.h"
#include "exporter/IChartExporter.h"
#include "logger/ILogger.h"
#include "style/IChartStyle.h"

/// Получение объекта нужного интерфейса (из IoC-контейнера).
template <typename T>
using ObjectFactory = std::function<std::shared_ptr<T>()>;

/// Чтение данных по расширению файла: "sqlite", "json".
using ReaderFactories = std::map<QString, ObjectFactory<IDataReader>>;

/// Типы графиков в порядке показа в списке: название → получение графика.
using ChartFactories = std::vector<std::pair<QString, ObjectFactory<IChart>>>;

/**
 * @brief Главное окно: список файлов слева, график и настройки справа.
 *
 * Окно знает только интерфейсы IDataReader, IChart, IChartStyle, IChartExporter и ILogger.
 * Все реализации регистрируются в IoC-контейнере в точке композиции (CreateMainWindow),
 * окно получает их готовыми через конструктор и ничего не регистрирует само.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief Конструктор: создаёт интерфейс окна.
     * @param logger     логгер.
     * @param readers    чтение данных по расширению файла.
     * @param charts     типы графиков.
     * @param colorStyle получение цветного стиля.
     * @param grayStyle  получение чёрно-белого стиля.
     * @param exporter   сохранение графика в файл.
     * @param parent     родительский виджет Qt.
     */
    MainWindow(std::shared_ptr<ILogger> logger, ReaderFactories readers, ChartFactories charts,
               ObjectFactory<IChartStyle> colorStyle, ObjectFactory<IChartStyle> grayStyle,
               std::shared_ptr<IChartExporter> exporter, QWidget* parent = nullptr);

private slots:
    /**
     * @brief Открывает диалог выбора папки и показывает её файлы.
     */
    void openFolder();

    /**
     * @brief Загружает данные из выбранного файла и строит график.
     *
     * Реализацию чтения берёт из словаря по расширению файла.
     * @param selected   выделенные элементы.
     * @param deselected элементы, с которых снято выделение (не используется).
     */
    void selectFile(const QItemSelection& selected, const QItemSelection& deselected);

    /**
     * @brief Берёт выбранный тип графика и перерисовывает график.
     * @param index номер пункта в списке типов.
     */
    void changeChartType(int index);

    /**
     * @brief Берёт цветной или чёрно-белый стиль и перерисовывает график.
     * @param gray true — чёрно-белый стиль.
     */
    void changeStyle(bool gray);

    /**
     * @brief Сохраняет текущий график в файл в выбранное пользователем место.
     */
    void printChart();

private:
    /**
     * @brief Строит график по текущим данным с текущими типом и стилем.
     */
    void redraw();

    std::shared_ptr<ILogger> logger;
    ReaderFactories readers;
    ChartFactories charts;
    ObjectFactory<IChartStyle> colorStyle;
    ObjectFactory<IChartStyle> grayStyle;
    std::shared_ptr<IChartExporter> exporter;

    std::shared_ptr<IChart> currentChart;
    std::shared_ptr<IChartStyle> currentStyle;
    QVector<DataPoint> points;

    QFileSystemModel* fileModel;
    QTableView* fileView;
    QComboBox* chartTypeBox;
    QCheckBox* grayCheck;
    QtCharts::QChartView* chartView;
};

#endif
