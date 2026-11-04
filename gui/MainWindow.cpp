#include "gui/MainWindow.h"
#include <QFileDialog>
#include <QFileInfo>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QPainter>
#include <QPushButton>
#include <QSplitter>
#include <QStatusBar>
#include <QVBoxLayout>

QT_CHARTS_USE_NAMESPACE

MainWindow::MainWindow(std::shared_ptr<ILogger> logger, ReaderFactories readers, ChartFactories charts,
                       ObjectFactory<IChartStyle> colorStyle, ObjectFactory<IChartStyle> grayStyle,
                       std::shared_ptr<IChartExporter> exporter, QWidget* parent)
    : QMainWindow(parent)
    , logger(logger)
    , readers(std::move(readers))
    , charts(std::move(charts))
    , colorStyle(std::move(colorStyle))
    , grayStyle(std::move(grayStyle))
    , exporter(exporter)
{
    // Начальные тип графика и стиль: первый в списке и цветной
    if (!this->charts.empty()) {
        currentChart = this->charts.front().second();
    }
    currentStyle = this->colorStyle();

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
    for (const auto& chart : this->charts) {
        chartTypeBox->addItem(chart.first);
    }
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
    auto reader = readers.find(suffix);
    if (reader == readers.end()) {
        statusBar()->showMessage("Формат не поддерживается: " + QFileInfo(path).fileName());
        return;
    }

    QVector<DataPoint> newPoints = reader->second()->read(path);
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
    if (index < 0 || index >= static_cast<int>(charts.size())) {
        return;
    }
    currentChart = charts[index].second();
    redraw();
}

void MainWindow::changeStyle(bool gray)
{
    currentStyle = gray ? grayStyle() : colorStyle();
    redraw();
}

void MainWindow::printChart()
{
    if (points.isEmpty()) {
        statusBar()->showMessage("Сначала выберите файл с данными");
        return;
    }

    QString path = QFileDialog::getSaveFileName(this, "Сохранить график",
                                                QDir::homePath() + "/" + exporter->defaultFileName(),
                                                exporter->fileFilter());
    if (path.isEmpty()) {
        return;
    }

    if (!exporter->exportChart(chartView, path)) {
        statusBar()->showMessage("Не удалось сохранить график");
        return;
    }
    statusBar()->showMessage("График сохранён: " + path);
}

void MainWindow::redraw()
{
    if (points.isEmpty() || !currentChart || !currentStyle) {
        return;
    }

    QChart* oldChart = chartView->chart();
    chartView->setChart(currentChart->buildGraf(points, *currentStyle));
    delete oldChart;
}
