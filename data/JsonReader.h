#ifndef JSONREADER_H
#define JSONREADER_H

#include <memory>
#include "data/IDataReader.h"
#include "logger/ILogger.h"

/**
 * @brief Чтение данных из JSON-файла.
 *
 * Формат файла — массив точек:
 * [ {"time": "01.01.2024", "value": 5.1}, ... ]
 */
class JsonReader : public IDataReader
{
public:
    /**
     * @brief Конструктор.
     * @param logger логгер для вывода сообщений.
     */
    explicit JsonReader(std::shared_ptr<ILogger> logger);

    /**
     * @brief Читает точки из JSON-файла.
     * @param path путь к файлу .json.
     * @return точки; при ошибке — пустой список.
     */
    QVector<DataPoint> read(const QString& path) override;

private:
    std::shared_ptr<ILogger> logger;
};

#endif
