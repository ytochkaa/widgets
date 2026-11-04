QT += core gui widgets charts sql

CONFIG += c++17

TARGET = widgets
TEMPLATE = app

INCLUDEPATH += $$PWD
DEFINES += $$shell_quote(TEST_DATA_DIR=\"$$PWD/test_data\")

SOURCES += \
    logger/ConsoleLogger.cpp \
    data/DataPoint.cpp \
    data/SqliteReader.cpp \
    data/JsonReader.cpp \
    style/ColorStyle.cpp \
    style/GrayStyle.cpp \
    chart/BarChart.cpp \
    chart/PieChart.cpp \
    chart/LineChart.cpp \
    gui/MainWindow.cpp \
    main.cpp

HEADERS += \
    ioc/IOCContainer.h \
    logger/ILogger.h \
    logger/ConsoleLogger.h \
    data/DataPoint.h \
    data/IDataReader.h \
    data/SqliteReader.h \
    data/JsonReader.h \
    style/IChartStyle.h \
    style/ColorStyle.h \
    style/GrayStyle.h \
    chart/IChart.h \
    chart/BarChart.h \
    chart/PieChart.h \
    chart/LineChart.h \
    gui/MainWindow.h
