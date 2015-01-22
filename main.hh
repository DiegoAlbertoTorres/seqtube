// Diego Torres Quintanilla
// Seqtube turn in 1.

#include <QDialog>
#include <QTextEdit>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QStyle>
#include <QStyleOptionFrameV2>
#include <QGridLayout>
#include <QVariantMap>
#include <QDataStream>
#include <QByteArray>
#include <QIODevice>
#include <QUdpSocket>
#include <QIcon>
#include <QHostInfo>
#include <QList>
#include <QTimer>
#include <QTime>
#include <QPair>
#include <QMap>
#include <QListWidget>
#include <QVBoxLayout>
#include <unistd.h>
#include <QApplication>
#include <QDebug>
#include <QPushButton>
#include <QFileDialog>
#include <QDir>
#include <QDataStream>
#include <QFileInfo>
#include <QQueue>
#include <sstream>
#include <QTableWidget>
#include <QHeaderView>
#include <QGroupBox>
#include <QProgressBar>
#include <QBitArray>
#include <Database.hh>
#include <QMutex>

#define CEILING(x,y) (((x) + (y) - 1) / (y))

#define STATUS_PERIOD   5000
#define ROUTE_PERIOD    60000
#define MONG_TIMEOUT    1000
#define TIMEOUT_NEXT    5

#define CHATHOPLIMIT    10
#define HASHSIZE        20   // bytes
#define BLOCKSIZE       8192  // bytes
#define BLOCKTIMEOUT    2000 // msec
#define HASHESPERBLOCK  CEILING(BLOCKSIZE,HASHSIZE)

#define FILE_INCOMPLETE 0
#define FILE_COMPLETE   1

#define DB_NOT_FOUND -2

// For message type map at ChatDialog::readNewMsg
enum keyType {
    SEQNO = 0,
    ORIGIN,
    LASTIP,
    LASTPORT,
    WANT,
    BLOCKREQUEST,
    DEST,
    HOPLIMIT,
    BLOCKREPLY,
    DATA,
    SEARCH,
    BUDGET,
    SEARCHREPLY,
    MATCHNAMES,
    MATCHIDS,
    MATCHSIZES,
    ISDATA,
    INDEX,
    NKEYTYPES
};

// For download list layout
enum columnNames {
    FILENAME_COLUMN = 0,
    STATUS_COLUMN,
    SIZE_COLUMN,
    PROGRESSBAR_COLUMN,
    NCOLUMNS
};

typedef QPair<QHostAddress, quint16> Peer;

QString sizeInUnits(quint64 sizeInt);

// Subclass for one line text inputs
class TextEdit : public QPlainTextEdit
{
    Q_OBJECT

    public:
        TextEdit();
        void keyPressEvent(QKeyEvent *event);
        QSize sizeHint();

    signals:
        void returnPressed();
};

class FileListItem : public QTableWidgetItem
{
    public:
        FileListItem(QString fileName, quint32 nPeers, quint64 size, quint32 id);

        QString fileName;
        quint32 nPeers;
        quint64 size;
        quint32 id;
};

class Block
{
    public:
        Block(bool isData, quint64 idx, QByteArray data);
        bool isData;
        quint64 idx;
        QByteArray data;
};

class MongMsg : public QObject
{
	Q_OBJECT
	
	public:
        MongMsg(Peer, QString, quint32, QString);
		
		QString origin;
		quint32 seqNo;
		QString chatText;
		QTimer timer;
        Peer inPeer;    // Received from
        Peer outPeer;   // Sent to
		
		void setTimer(int time);
		void stopTimer();
		void print();
		
	private:
		

	public slots:
		void sendTimeout();
	
	signals:
		void timeout(MongMsg*);
};

class SharedFile : public QObject
{
    Q_OBJECT

    public:
        SharedFile(QString, quint64, QByteArray, quint32);

         QString name;
         quint64 size;
         QByteArray hashHead; // Main hash of file
         quint32 id;          // Id of file, used by DB

    private:

         QList<Peer> peers;

    public slots:

    signals:
};

class FileDownload;

// Class to store data for specific packet requests
class BlockRequest : public QObject
{
    Q_OBJECT

    public:
        BlockRequest(FileDownload* parent, QByteArray hash, QString source, QString priority, quint16 maxClock);

        FileDownload *parent;   // Download to which block belongs
        QByteArray hash;        // Hash of block
        QString source;         // Block requested from this source
        QString priority;       // Priority string
        quint16 clock;          // Manually ticked on packet receives
        quint16 maxClock;       // Timeout for clock
        QTimer timer;           // Time kept timer

    public slots:
        void sendTimeout();

    signals:
        void timeout(BlockRequest*, FileDownload*);

};

// Class for file downloads
class FileDownload : public QObject
{
    Q_OBJECT

    public:
        FileDownload(QString fileName, QString path, quint64 size, QByteArray hashHead, QList<QString> peers);
        ~FileDownload();

        // File metadata
        QString fileName;
        QString path;
        quint64 size;
        QByteArray hashHead;

        QMap<QString, QByteArray> blockQ;               // Block priority queue
        QMap<QByteArray, QString> hashToPriority;
        QHash<QByteArray, BlockRequest*> pendingReqs;   // Requests pending from peers
        QList<QString> peers;
        QList<QString> freePeers;
        QBitArray fileMap;                              // Bitmap: blocks received so far
        FileListItem *listItem;
};

class SearchDialog;

class ChatDialog : public QDialog
{
	Q_OBJECT

	public:
		ChatDialog();

		QString host;
		int port;
		bool forward;
        quint32 myHopLimit;
        QString downloadPath;
        QMap<QByteArray, FileDownload*> fileDownloads;

        void setPort(int p);
        void setForwarding(bool set);
        void initNeighbors(QList<quint16>* ports);

        // Route table handlers
        void putOnTable(QString origin, Peer pair);
		void removeFromTable(QString origin);
        Peer getFromTable(QString origin);

        void printNeighbors();

	public slots:
        // Message protocol functions
        void rumorTimeout(MongMsg* msg);
        void readNewMsg(QByteArray* bytes, Peer inpeer);
		
        // Handling peers/neighbors
		void newNeighInput();
        void resolveNeighbor(QHostInfo hostInfo);
        void processNewNeigh(QString input);
        void addIfNewPeer(Peer candidate);
		
        // Packet sending
        void sendStatus(Peer outPeer= Peer());
        void sendRoute(Peer outPeer= Peer());
        void sendBlockRequest(Peer inPeer, QString dest, QString origin, quint32 hopLimit, QByteArray blockRequest);
        void sendBlockReply(Peer inPeer, QString dest, QString origin, quint32 hopLimit, QByteArray blockReply, QByteArray blockData, bool isData, quint64 idx);
        void sendSearchReply(Peer inPeer, QString dest, QString origin, quint32 hopLimit, QString searchReply, QVariantList matchNames, QVariantList matchIds, QVariantList matchSizes);
        void sendSearchRequest(Peer, QString, QString, quint32);
        void sendPacket(QVariantMap packet, Peer outPeer);

        // Downloads and file sharing
        void showShareFileDialog();
        void shareFiles(QStringList files);
        void startFileDownload(QString fileName, quint64 size, QByteArray hashHead, QList<QString> peers);
        void requestTimeout(BlockRequest *req, FileDownload *download);
        void loadSharedFromDB(QString filename, quint64 size, QByteArray hashHead, quint32 id);
        void deleteSelectedFiles();

	private:
        // GUI
		QGridLayout *layout;
		QLineEdit *textNeigh;
		QListWidget *chatList;
        QPushButton *shareFileButton;
        QPushButton *deleteFileButton;
        QFileDialog *shareFileDialog;
        SearchDialog *searchDialog;
        QTableWidget *fileList;

        // State
        QVariantMap status;
		quint32 msgCounter;	
		QTimer statusTimer;
		QTimer routeTimer;
		
        // Peers
        QList<Peer > neighbors;
        QMap<int, Peer > unresolvedNeighbors;
        QHash<QString, Peer > routeTable;

        // Storage
        Database *db;
        QMap<QString, QMap<quint32, MongMsg*> > msgArchive;
        QMap<quint32, SharedFile*> sharedFiles;
        QList<MongMsg*> onHold;
        QMap<QByteArray, FileDownload*> hashToFile;
        QHash<QString, quint8> keyToType;
		
        // Initialization functions
        void initKeyMap();

        // Status handling
        bool compareStatus(QVariantMap, Peer);
        bool addToStatus(QString, quint32, int isMsg = 0);
		
        // Message sending
		void broadcastToAll(MongMsg*);
        void serializeMsg(MongMsg*, Peer outPeer);
		
        Peer randomNeighbor(Peer except1 = Peer(), Peer except2 = Peer());
		
        // Archive handlers
		MongMsg* getFromArchive(QString, quint32);
        void putOnArchive(MongMsg *msg);
		
		void monger(MongMsg*);
		
        // On hold handlers
		void putOnHold(MongMsg *msg);
        void removeFromHold(Peer);
        bool isOnHold(Peer);
        MongMsg* getFromHold(Peer);

        // Message type handlers
        void handleRoute(Peer inPeer, bool isNew, bool isDirect, QString origin, quint32 seqNo, QHostAddress lastIP, quint16 lastPort);
        void handleStatus(Peer inPeer, QVariantMap hisStatus);
        void handleBlockRequest(Peer inPeer, QString dest, QString origin, quint32 hopLimit, QByteArray blockRequest);
        void handleBlockReply(QString dest, QString origin, quint32 hopLimit, QByteArray blockReply, QByteArray blockData, bool isData, quint64 idx);
        void handleSearchRequest(Peer inPeer, QString origin, quint32 budget, QString search);
        void handleSearchReply(Peer inPeer, QString dest, QString origin, quint32 hopLimit, QString searchReply, QVariantList matchNames, QVariantList matchIds, QVariantList sizes);

        // Functions to create shared files
        void fillQ0(QDataStream&, QQueue<QByteArray>&, qint64 &position);
        QByteArray hashQueue(QQueue<QByteArray>&);
        void buildSharedFile(QString filePath);
        QByteArray buildMerkleTree(QDataStream &in);
        FileListItem* putOnFileList(int status, QString fileName, quint64 size, quint32 id, int nPeers=0);

        // Functions to control downloads
        void clockRequests(FileDownload *download);
        void makeBlockRequest(FileDownload* download, QString dest, QString priority, QByteArray blockHash);
        void requestNth(QString dest, FileDownload *download, quint32 n);
        void resolveDownload(FileDownload* download);
        void updateProgressBar(FileDownload* download, quint64 idx);
        void writeBlock(FileDownload* download, QByteArray data, quint64 idx);
        void employPeers(FileDownload* download);
        void enqueueMetadata(FileDownload* download, BlockRequest* req, QByteArray blockData);

	signals:
		void outmsgReady(QByteArray*, QHostAddress, int);
};

// Subclass of table items, for the search tables
class SearchResult : public QTableWidgetItem
{
    public:
        SearchResult(QString fileName, QString peer, QByteArray metaHash, quint64 size);

        QSet<QString> peers;
        QString fileName;
        QString searchTerm;
        QByteArray metaHash;
        quint64 size;
};

// Search table and search bar/button all part of a group. So, can be
// easily moved to a QDialog.
class SearchDialog : public QGroupBox
{
    Q_OBJECT

    public:
        SearchDialog(QString title, ChatDialog* prnt);

        QTableWidget *resultList;
        QString currentSearch;

        void addResult(QString fileName, QString origin, QByteArray hash, quint64 size);

    public slots:
        void search();
        void expandSearch();
        void startDownload();

    private:
        ChatDialog* dad;
        QLineEdit *searchInput;
        QPushButton *searchButton;
        QPushButton *closeButton;
        QGridLayout *layout;
        QTimer timer;
        quint32 budget;

    signals:
        void searchRequested(QString);
};

// Socket for data transfer accross UDP
class NetSocket : public QUdpSocket
{
	Q_OBJECT

public:
	NetSocket();

	// Bind this socket to a Peerster-specific default port.
	bool bind();
	int getPort();
	QList<quint16>* getUDPPorts();
	
public slots:
	void broadcastMsg(QByteArray*, QHostAddress, int);
	void receiveMsg();
	
signals:
    void msgReceived(QByteArray*, Peer);
    void gotNewPeer(Peer);

private:
	int myPortMin, myPortMax;
	int port;
};
