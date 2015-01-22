#include "main.hh"

FileDownload::FileDownload(QString fileName, QString path, quint64 size, QByteArray hashHead, QList<QString> peers)
{
    this->fileName = fileName;
    this->path = path;
    this->size = size;
    this->hashHead = hashHead;
    this->peers = peers;
    this->freePeers = peers;
    this->fileMap.resize(CEILING(size,BLOCKSIZE));
}

// Delete data in pendingReqs
FileDownload::~FileDownload(){
    qDeleteAll(pendingReqs);
}
