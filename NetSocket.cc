#include "main.hh"

NetSocket::NetSocket()
{
	// Pick a range of four UDP ports to try to allocate by default,
	// computed based on my Unix user ID.
	// This makes it trivial for up to four Peerster instances per user
	// to find each other on the same host,
	// barring UDP port conflicts with other applications
	// (which are quite possible).
	// We use the range from 32768 to 49151 for this purpose.
	myPortMin = 32768 + (getuid() % 4096)*4;
	myPortMax = myPortMin + 3;
}

bool NetSocket::bind()
{
	// Try to bind to each of the range myPortMin..myPortMax in turn.
	for (int p = myPortMin; p <= myPortMax; p++) {
		if (QUdpSocket::bind(p)) {
			port = p;
			//~ qDebug() << "UDP port:" << p;
			return true;
		}
	}

	qDebug() << "No ports in my default range " << myPortMin
		<< "-" << myPortMax << " available";
	return false;
}

void NetSocket::broadcastMsg(QByteArray *msg, QHostAddress address, int destination)
{	
	// Send everywhere
	if (destination < 0){
		for (quint16 x = myPortMin; x <= myPortMax; x++)
			writeDatagram(*msg, QHostAddress(QHostAddress::LocalHost), x);
		//delete msg;
	}
	// Send to specific port
	else{
		writeDatagram(*msg, address, destination);
		//delete msg;
	}
}

void NetSocket::receiveMsg()
{
	while (hasPendingDatagrams()) {
		QByteArray *datagram = new QByteArray;
        QHostAddress senderIP;
		quint16 senderPort;
        Peer sender;

        datagram->resize(pendingDatagramSize());
        readDatagram(datagram->data(), datagram->size(), &senderIP, &senderPort);

        sender = Peer(senderIP, senderPort);

        emit gotNewPeer(sender);
        emit msgReceived(datagram, sender); // Connected to ChatDailog::readNewMsg
     }
}

QList<quint16>* NetSocket::getUDPPorts()
{
	QList<quint16>* neighbors = new QList<quint16>;
	
	if (port == myPortMin)
		(*neighbors) << (port+1);
	else if (port == myPortMax)
		(*neighbors) << (port-1);
	else if (port > myPortMin && port < myPortMax)
		(*neighbors) << (port-1) << (port+1);
	else{
		qDebug() << "Port not in valid range.";
		exit(1);
	}
	
	return neighbors;	
}

int NetSocket::getPort()
{
	return port;
}
