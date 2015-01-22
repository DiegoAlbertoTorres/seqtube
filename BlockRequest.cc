#include "main.hh"

BlockRequest::BlockRequest(FileDownload* parent, QByteArray hash, QString source, QString priority, quint16 maxClock)
{
    this->parent = parent;
    this->hash = hash;
    this->source = source;
    this->priority = priority;
    this->clock = 0;
    this->maxClock = maxClock;
    timer.setInterval(BLOCKTIMEOUT);

    connect(&timer, SIGNAL(timeout()), this, SLOT(sendTimeout()));
}

void BlockRequest::sendTimeout(){
    emit timeout(this, parent);
}
