#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QObject>
#include <QHostInfo>

#define FILE_INCOMPLETE 0
#define FILE_COMPLETE   1

#define DB_NOT_FOUND -2

class Database : public QObject
{
    Q_OBJECT

    public:
        Database(QObject *parent = 0);
        bool openDB();
        bool deleteDB();
        QSqlError lastError();
        bool setUpDataTable();
        bool setUpFileTable();
        quint32 insertFile(QString fileName, quint64 size, QByteArray hashHead);
        bool insertData(quint32 id, QByteArray hash, QByteArray data, qint64 idx);
        QPair<qint64, QByteArray> get(QByteArray hash);
        bool execDataInserts();
        bool deleteFile(quint32 id);

    private:
        QSqlDatabase db;
        quint32 cur_id;

        QVariantList pendingHashes;
        QVariantList pendingIdxs;
        QVariantList pendingData;
        QVariantList pendingIds;

    signals:
        void fileFound(QString, quint64, QByteArray, quint32);
};

#endif // DATABASE_H
