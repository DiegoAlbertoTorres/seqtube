#include "main.hh"

MongMsg::MongMsg(QPair<QHostAddress, quint16> inPeer, QString origin, quint32 seqNo, QString text)
{
    this->inPeer = inPeer;
	this->origin = origin;
	this->seqNo = seqNo;
	this->chatText = QString(text);
    this->outPeer = QPair<QHostAddress, quint16>();

	timer.setSingleShot(true);
	connect(&timer, SIGNAL(timeout()), this, SLOT(sendTimeout()));
}

void MongMsg::print(void)
{
    qDebug() << "Message from:" << inPeer;
	qDebug() << "Message is:" << origin << seqNo << chatText;
}
void MongMsg::setTimer(int time)
{
	timer.start(time);
}

void MongMsg::stopTimer()
{
	timer.stop();
}

void MongMsg::sendTimeout()
{
	emit timeout(this);
}
