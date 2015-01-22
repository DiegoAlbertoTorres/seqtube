#include "Database.hh"

Database::Database(QObject *parent) : QObject(parent)
{
    this->cur_id = 0;
    QObject::connect(this, SIGNAL(fileFound(QString, quint64, QByteArray, quint32)), parent, SLOT(loadSharedFromDB(QString, quint64, QByteArray, quint32)));
}

bool Database::openDB()
{
    // Find QSLite driver
    db = QSqlDatabase::addDatabase("QSQLITE");

//    db.setDatabaseName("seqtube-db" + QString::number(qrand() % 1000) + ".sqlite");
    db.setDatabaseName("seqtube-db-" + QHostInfo::localHostName() + ".sqlite");

    // Open databasee
    return db.open();
}

bool Database::setUpDataTable()
{
    // Create if theres no data table
    if (!(db.tables().contains("fileData"))){
        QSqlQuery query;
        query.prepare("CREATE TABLE fileData (hash, idx, data, id)");
        if (!query.exec()){
            qDebug() << query.lastError();
            return false;
        }

        query.prepare("DROP TABLE files");
        query.exec();
    }
    return true;
}

bool Database::setUpFileTable()
{
    // Read existing
    if (db.tables().contains("files")){
        QSqlQuery query;
        query.prepare("SELECT * FROM files");
        query.exec();
        while (query.next()){
            if (query.value(0).toInt() > cur_id)
                cur_id = query.value(0).toInt() + 1;
            emit fileFound(query.value(1).toString(), query.value(2).toULongLong(), query.value(3).toByteArray(), query.value(0).toUInt());
        }
    }
    // Create
    else{
        QSqlQuery query;
        query.prepare("CREATE TABLE files (id, fileName, size, hashHead)");
        if (!query.exec()){
            qDebug() << query.lastError();
            return false;
        }
    }
    return true;
}

quint32 Database::insertFile(QString fileName, quint64 size, QByteArray hashHead)
{
    QSqlQuery query;
    query.prepare("INSERT INTO files (id, fileName, size, hashHead) VALUES (:id, :fileName, :size, :hashHead)");
    query.bindValue(":id", QVariant(cur_id++));
    query.bindValue(":fileName", QVariant(fileName));
    query.bindValue(":size", QVariant(size));
    query.bindValue(":hashHead", QVariant(hashHead));
    query.exec();
    return cur_id;
}

bool Database::deleteFile(quint32 id)
{
    db.transaction();
    QSqlQuery deleteFromFiles, deleteData;
    deleteFromFiles.prepare("DELETE FROM files WHERE id=:thisId");
    deleteFromFiles.bindValue(":thisId", id);
    deleteFromFiles.exec();

    deleteData.prepare("DELETE FROM fileData WHERE id=:thisId");
    deleteData.bindValue(":thisId", id);
    deleteData.exec();
    db.commit();
    return true;
}

bool Database::insertData(quint32 id, QByteArray hash, QByteArray data, qint64 idx)
{
    pendingHashes << QVariant(hash);
    pendingIdxs << QVariant(idx);
    pendingData << QVariant(data);
    pendingIds << QVariant(id);

    if (pendingHashes.size() > 500){
        execDataInserts();
    }

    return true;
}

bool Database::execDataInserts()
{
    db.transaction();
    QSqlQuery dataQuery;
    dataQuery.prepare("INSERT INTO fileData (hash, idx, data, id) VALUES (?, ?, ?, ?)");
    dataQuery.addBindValue(pendingHashes);
    dataQuery.addBindValue(pendingIdxs);
    dataQuery.addBindValue(pendingData);
    dataQuery.addBindValue(pendingIds);

    if (!dataQuery.execBatch()){
        return false;
        qDebug() << dataQuery.lastError();
    }
    db.commit();

    pendingHashes.clear();
    pendingIdxs.clear();
    pendingData.clear();
    pendingIds.clear();
    return true;
}

QPair<qint64, QByteArray> Database::get(QByteArray hash)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM fileData WHERE hash=:thisHash");
    query.bindValue(":thisHash", hash);
    query.exec();
    if (query.first()){
        return QPair<qint64, QByteArray>(query.value(1).toInt(), query.value(2).toByteArray());
    }
    else{
        return QPair<qint64, QByteArray>(DB_NOT_FOUND, QByteArray());
    }
}

QSqlError Database::lastError()
    {
    // If opening database has failed user can ask
    // error description by QSqlError::text()
    return db.lastError();
    }

bool Database::deleteDB()
{
    // Close database
    db.close();

    // NOTE: We have to store database file into user home folder in Linux
    QString path(QDir::home().path());
    path.append(QDir::separator()).append("my.db.sqlite");
    path = QDir::toNativeSeparators(path);
    return QFile::remove(path);
}
