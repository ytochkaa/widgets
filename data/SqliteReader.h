#ifndef SQLITEREADER_H
#define SQLITEREADER_H

#include <memory>
#include "data/IDataReader.h"
#include "logger/ILogger.h"

/**
 * @brief Чтение данных из базы SQLite.
 *
 * Берётся первая таблица базы, из неё читаются столбцы Time и Value.
 */
class SqliteReader : public IDataReader
{
public:
    /**
     * @brief Конструктор.
     * @param logger логгер для вывода сообщений.
     */
    explicit SqliteReader(std::shared_ptr<ILogger> logger);

    /**
     * @brief Читает точки из первой таблицы базы.
     * @param path путь к файлу .sqlite.
     * @return точки; при ошибке — пустой список.
     */
    QVector<DataPoint> read(const QString& path) override;

private:
    std::shared_ptr<ILogger> logger;
};

#endif
