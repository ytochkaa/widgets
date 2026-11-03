cmake_minimum_required(VERSION 3.16)
project(widgets LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Charts Sql)

add_executable(${PROJECT_NAME}
    ioc/IOCContainer.h

    logger/ILogger.h
    logger/ConsoleLogger.h
    logger/ConsoleLogger.cpp

    data/DataPoint.h
    data/DataPoint.cpp
    data/IDataReader.h
    data/SqliteReader.h
    data/SqliteReader.cpp
    data/JsonReader.h
    data/JsonReader.cpp

    style/IChartStyle.h
    style/ColorStyle.h
    style/ColorStyle.cpp
    style/GrayStyle.h
    style/GrayStyle.cpp

    chart/IChart.h
    chart/BarChart.h
    chart/BarChart.cpp
    chart/PieChart.h
    chart/PieChart.cpp
    chart/LineChart.h
    chart/LineChart.cpp

    gui/MainWindow.h
    gui/MainWindow.cpp

    main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        Qt5::Widgets
        Qt5::Charts
        Qt5::Sql
)
