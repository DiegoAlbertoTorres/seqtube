#include "main.hh"
#include <QtCrypto>

int main(int argc, char **argv)
{	
	// Initialize Qt toolkit
	QApplication app(argc,argv);
	
	qDebug() << "************************************";
	
	QList<QString> cmdArguments = QCoreApplication::arguments();
	QList<QString>::iterator i;
	
	QTime now = QTime::currentTime();
	qsrand(now.msec());

	// Create an initial chat dialog window
	ChatDialog dialog;
	
	// Crypto initializer
	QCA::Initializer init;

	// Create a UDP network socket
	NetSocket sock;
	if (!sock.bind())
		exit(1);

    dialog.setPort(sock.getPort());
    dialog.initNeighbors(sock.getUDPPorts());
	
    // Add command-line specified neighbors
	i = cmdArguments.begin();
	for (i += 1; i != cmdArguments.end(); ++i){
		if (*i == "-noforward"){
			dialog.setForwarding(false);
			qDebug() << "Forwarding off";
		}
		else
			dialog.processNewNeigh(*i);
	}

	// Send route message on startup to random neighbor
    dialog.sendStatus();
	dialog.sendRoute();
		
	dialog.show();
		
	QObject::connect(&dialog, SIGNAL(outmsgReady(QByteArray*, QHostAddress, int)), &sock, SLOT(broadcastMsg(QByteArray*, QHostAddress, int)));
	QObject::connect(&sock, SIGNAL(readyRead()), &sock, SLOT(receiveMsg()));
    QObject::connect(&sock, SIGNAL(msgReceived(QByteArray*, Peer)), &dialog, SLOT(readNewMsg(QByteArray*, Peer)));
    QObject::connect(&sock, SIGNAL(gotNewPeer(Peer)), &dialog, SLOT(addIfNewPeer(Peer)));

	// Enter the Qt main loop; everything else is event driven
	return app.exec();
}

