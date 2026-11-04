#include "gui/MainWindow.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPainter>
#include <QPdfWriter>
#include <QPushButton>
#include <QSplitter>
#include <QStatusBar>
#include <QVBoxLayout>
#include "chart/BarChart.h"
#include "chart/IChart.h"
#include "chart/LineChart.h"
#include "chart/PieChart.h"
#include "data/IDataReader.h"
#include "data/JsonReader.h"
#include "data/SqliteReader.h"
#include "style/ColorStyle.h"
#include "style/GrayStyle.h"
#include "style/IChartStyle.h"

QT_CHARTS_USE_NAMESPACE

MainWindow::MainWindow(IOCContainer& container, QWidget* parent)
    : QMainWindow(parent), container(container), logger(container.GetObject<ILogger>())
{
    setWindowTitle("PrintChart");
    resize(1400, 700);

    // Левая часть: кнопка выбора папки и список файлов
    QPushButton* folderButton = new QPushButton("Открыть папку");

    // Стартовая папка: test_data из проекта, если она есть, иначе домашняя папка
    QString homePath = QDir::homePath();
    if (QDir(TEST_DATA_DIR).exists()) {
        homePath = TEST_DATA_DIR;
    }
    fileModel = new QFileSystemModel(this);
    fileModel->setFilter(QDir::NoDotAndDotDot | QDir::Files);
    fileModel->setRootPath(homePath);

    fileView = new QTableView();
    fileView->setModel(fileModel);
    fileView->setRootIndex(fileModel->index(homePath));
    fileView->setSelectionBehavior(QAbstractItemView::SelectRows);
    fileView->setSelectionMode(QAbstractItemView::SingleSelection);
    fileView->setColumnWidth(0, 200);

    QWidget* leftPart = new QWidget();
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPart);
    leftLayout->addWidget(folderButton);
    leftLayout->addWidget(fileView);

    // Правая часть: настройки графика и сам график
    chartTypeBox = new QComboBox();
    chartTypeBox->addItems({"BarChart", "PieChart", "LineChart"});
    grayCheck = new QCheckBox("Черно-белый график");
    QPushButton* printButton = new QPushButton("Печать графика");

    QHBoxLayout* settingsLayout = new QHBoxLayout();
    settingsLayout->addStretch();
    settingsLayout->addWidget(new QLabel("Выберите тип диаграммы"));
    settingsLayout->addWidget(chartTypeBox);
    settingsLayout->addWidget(grayCheck);
    settingsLayout->addWidget(printButton);
    settingsLayout->addStretch();

    chartView = new QChartView();
    chartView->setRenderHint(QPainter::Antialiasing);

    QWidget* rightPart = new QWidget();
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPart);
    rightLayout->addLayout(settingsLayout);
    rightLayout->addWidget(chartView);

    QSplitter* splitter = new QSplitter(this);
    splitter->addWidget(leftPart);
    splitter->addWidget(rightPart);
    splitter->setSizes({450, 950});
    setCentralWidget(splitter);

    statusBar()->showMessage("Выберите файл БД");

    connect(folderButton, &QPushButton::clicked, this, &MainWindow::openFolder);
    connect(fileView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::selectFile);
    connect(chartTypeBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::changeChartType);
    connect(grayCheck, &QCheckBox::toggled, this, &MainWindow::changeStyle);
    connect(printButton, &QPushButton::clicked, this, &MainWindow::printChart);
}

void MainWindow::openFolder()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Выбор папки", fileModel->rootPath());
    if (dir.isEmpty()) {
        return;
    }
    fileView->setRootIndex(fileModel->setRootPath(dir));
    statusBar()->showMessage("Выберите файл БД");
    logger->logInfo("Открыта папка " + dir);
}

void MainWindow::selectFile(const QItemSelection& selected, const QItemSelection& deselected)
{
    Q_UNUSED(deselected);

    QModelIndexList indexes = selected.indexes();
    if (indexes.isEmpty()) {
        return;
    }
    QString path = fileModel->filePath(indexes.first());
    QString suffix = QFileInfo(path).suffix().toLower();

    // Выбираем реализацию чтения по расширению файла
    if (suffix == "sqlite") {
        container.RegisterFactory<IDataReader, SqliteReader, ILogger>();
    } else if (suffix == "json") {
        container.RegisterFactory<IDataReader, JsonReader, ILogger>();
    } else {
        statusBar()->showMessage("Формат не поддерживается: " + QFileInfo(path).fileName());
        return;
    }

    QVector<DataPoint> newPoints = container.GetObject<IDataReader>()->read(path);
    if (newPoints.isEmpty()) {
        statusBar()->showMessage("Не удалось прочитать данные из " + QFileInfo(path).fileName());
        return;
    }

    points = newPoints;
    statusBar()->showMessage(QString("Файл: %1, точек: %2").arg(QFileInfo(path).fileName()).arg(points.size()));
    redraw();
}

void MainWindow::changeChartType(int index)
{
    if (index == 0) {
        container.RegisterFactory<IChart, BarChart>();
    } else if (index == 1) {
        container.RegisterFactory<IChart, PieChart>();
    } else {
        container.RegisterFactory<IChart, LineChart>();
    }
    redraw();
}

void MainWindow::changeStyle(bool gray)
{
    if (gray) {
        container.RegisterInstance<IChartStyle, GrayStyle>();
    } else {
        container.RegisterInstance<IChartStyle, ColorStyle>();
    }
    redraw();
}

void MainWindow::printChart()
{
    if (points.isEmpty()) {
        statusBar()->showMessage("Сначала выберите файл с данными");
        return;
    }

    QString path = QFileDialog::getSaveFileName(this, "Сохранить график", QDir::homePath() + "/chart.pdf",
                                                "PDF (*.pdf)");
    if (path.isEmpty()) {
        return;
    }

    QPdfWriter writer(path);
    writer.setPageSize(QPageSize(QPageSize::A4));
    writer.setPageOrientation(QPageLayout::Landscape);

    QPainter painter(&writer);
    if (!painter.isActive()) {
        logger->logError("Не удалось создать PDF " + path);
        statusBar()->showMessage("Не удалось сохранить PDF");
        return;
    }
    chartView->render(&painter);
    painter.end();

    statusBar()->showMessage("График сохранён: " + path);
    logger->logInfo("График сохранён в " + path);
}

void MainWindow::redraw()
{
    if (points.isEmpty()) {
        return;
    }

    std::shared_ptr<IChart> chart = container.GetObject<IChart>();
    std::shared_ptr<IChartStyle> style = container.GetObject<IChartStyle>();

    QChart* oldChart = chartView->chart();
    chartView->setChart(chart->buildGraf(points, *style));
    delete oldChart;
}
