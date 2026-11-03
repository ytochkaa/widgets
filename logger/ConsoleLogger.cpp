#include "logger/ConsoleLogger.h"
#include <QDateTime>
#include <iostream>

static QString makeTimestamp()
{
    return QDateTime::currentDateTime().toString(Qt::ISODate);
}

void ConsoleLogger::logInfo(const QString& message)
{
    std::cout << makeTimestamp().toStdString() << " [INFO] " << message.toStdString() << "\n";
    std::cout.flush();
}

void ConsoleLogger::logError(const QString& message)
{
    std::cout << makeTimestamp().toStdString() << " [ERROR] " << message.toStdString() << "\n";
    std::cout.flush();
}
