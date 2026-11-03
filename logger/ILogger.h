#ifndef ILOGGER_H
#define ILOGGER_H

#include <QString>

/**
 * @brief Абстрактный интерфейс логирования событий приложения.
 */
class ILogger
{
public:
    virtual ~ILogger() = default;

    /**
     * @brief Записывает информационное сообщение.
     * @param message текст сообщения.
     */
    virtual void logInfo(const QString& message) = 0;

    /**
     * @brief Записывает сообщение об ошибке.
     * @param message описание ошибки.
     */
    virtual void logError(const QString& message) = 0;
};

#endif
