#include "main.hh"

FileListItem::FileListItem(QString fileName, quint32 nPeers, quint64 size, quint32 id) : QTableWidgetItem(fileName)
{
    this->fileName = fileName;
    this->nPeers = nPeers;
    this->size = size;
    this->id = id;

    this->setFlags(this->flags() & ~(Qt::ItemIsEditable | Qt::ItemIsUserCheckable));
}
