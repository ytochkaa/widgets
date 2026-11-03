#ifndef CONSOLELOGGER_H
#define CONSOLELOGGER_H

#include "logger/ILogger.h"

/**
 * @brief Реализация ILogger, выводящая сообщения в консоль через std::cout.
 */
class ConsoleLogger : public ILogger
{
public:
    /**
     * @brief Выводит информационное сообщение с отметкой времени и тегом [INFO].
     * @param message текст сообщения.
     */
    void logInfo(const QString& message) override;

    /**
     * @brief Выводит сообщение об ошибке с отметкой времени и тегом [ERROR].
     * @param message описание ошибки.
     */
    void logError(const QString& message) override;
};

#endif
