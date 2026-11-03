#include "data/JsonReader.h"
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>

JsonReader::JsonReader(std::shared_ptr<ILogger> logger) : logger(logger) {}

QVector<DataPoint> JsonReader::read(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        logger->logError("Не удалось открыть файл " + path);
        return {};
    }

    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &error);
    if (error.error != QJsonParseError::NoError) {
        logger->logError("Ошибка в JSON " + path + ": " + error.errorString());
        return {};
    }
    if (!doc.isArray()) {
        logger->logError("JSON должен быть массивом точек: " + path);
        return {};
    }

    QVector<DataPoint> points;
    QJsonArray array = doc.array();
    for (int i = 0; i < array.size(); ++i) {
        QJsonObject obj = array[i].toObject();
        if (!obj["time"].isString() || !obj["value"].isDouble()) {
            logger->logError(QString("Неверная точка №%1 в %2: нужны поля time (строка) и value (число)")
                                 .arg(i + 1)
                                 .arg(path));
            return {};
        }
        points.append({obj["time"].toString(), obj["value"].toDouble()});
    }

    logger->logInfo(QString("Прочитано %1 точек из %2").arg(points.size()).arg(path));
    return points;
}
