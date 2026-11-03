#ifndef IDATAREADER_H
#define IDATAREADER_H

#include "data/DataPoint.h"

/**
 * @brief Абстрактный интерфейс чтения данных [дата, значение] из файла.
 */
class IDataReader
{
public:
    virtual ~IDataReader() = default;

    /**
     * @brief Читает файл и возвращает список точек.
     * @param path путь к файлу.
     * @return точки; при ошибке — пустой список (причина пишется в лог).
     */
    virtual QVector<DataPoint> read(const QString& path) = 0;
};

#endif
