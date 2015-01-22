#include "main.hh"

SharedFile::SharedFile(QString name, quint64 size, QByteArray hashHead, quint32 id){
    this->name = name;
    this->size = size;
    this->hashHead = hashHead;
    this->id = id;
}
