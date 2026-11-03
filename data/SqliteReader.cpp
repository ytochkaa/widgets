#include "data/SqliteReader.h"
#include <QFile>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QStringList>
#include <QVariant>

static const QString kConnectionName = "SqliteReader";

SqliteReader::SqliteReader(std::shared_ptr<ILogger> logger) : logger(logger) {}

QVector<DataPoint> SqliteReader::read(const QString& path)
{
    if (!QFile::exists(path)) {
        logger->logError("Файл не найден: " + path);
        return {};
    }

    QVector<DataPoint> points;
    {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", kConnectionName);
        db.setDatabaseName(path);
        db.setConnectOptions("QSQLITE_OPEN_READONLY");

        if (!db.open()) {
            logger->logError("Не удалось открыть базу " + path + ": " + db.lastError().text());
        } else {
            QStringList tables = db.tables();
            if (tables.isEmpty()) {
                logger->logError("В базе нет таблиц или это не база SQLite: " + path);
            } else {
                QSqlQuery query(db);
                if (!query.exec("SELECT Time, Value FROM \"" + tables.first() + "\"")) {
                    logger->logError("Ошибка чтения таблицы " + tables.first() + ": " + query.lastError().text());
                } else {
                    while (query.next()) {
                        points.append({query.value(0).toString(), query.value(1).toDouble()});
                    }
                    logger->logInfo(QString("Прочитано %1 точек из таблицы %2").arg(points.size()).arg(tables.first()));
                }
            }
            db.close();
        }
    }
    QSqlDatabase::removeDatabase(kConnectionName);
    return points;
}
