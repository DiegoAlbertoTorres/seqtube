#include "main.hh"
#include "sha1sum.hh"
#include "Database.hh"

ChatDialog::ChatDialog()
{
    // Initializations
    // Host is localhost and a 4 digit random number
    host = QHostInfo::localHostName() + "-" + QString::number(qrand() % 10000);
	forward = true;
    downloadPath = QString(QDir::homePath() + "/Desktop/");
    myHopLimit = CHATHOPLIMIT;
	msgCounter = 1;
    initKeyMap();
	
	qDebug() << "Host:" << host;
    qDebug() << "Download path:" << downloadPath;

    // Widgets
	textNeigh = new QLineEdit(this);
	chatList = new QListWidget(this);
    shareFileButton = new QPushButton(QString("Share file"), this);
    deleteFileButton = new QPushButton(QString("Delete selected"), this);
    shareFileDialog = new QFileDialog(this);
    searchDialog = new SearchDialog(QString("File Search"), this);
    fileList = new QTableWidget(this);

    // Set widget properties
    textNeigh->setPlaceholderText("New peers in format \"Address:Port\"");
	chatList->setFixedWidth(180);
    shareFileDialog->setFileMode(QFileDialog::ExistingFiles);
    shareFileDialog->setDirectory(QDir::homePath());

    shareFileButton->setAutoDefault(false);
    deleteFileButton->setAutoDefault(false);
    shareFileButton->setDefault(false);
    deleteFileButton->setDefault(false);

    // Download List properties
    fileList->verticalHeader()->setVisible(false);
    fileList->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    fileList->setColumnCount(NCOLUMNS);
    fileList->setSelectionBehavior(QAbstractItemView::SelectRows);
    fileList->setSelectionMode(QAbstractItemView::SingleSelection);
    fileList->setHorizontalHeaderLabels(QStringList() << "File Name"
                                            << "Status" << "File Size" << "Progress");

	// Lay out the widgets to appear in the main window.
	// For Qt widget and layout concepts see:
	// http://doc.qt.nokia.com/4.7-snapshot/widgets-and-layouts.html
    layout = new QGridLayout(this);

    // Row, column, rowspan, colspan, alignment
    // Column 0 - Search Dialog
    layout->addWidget(searchDialog, 0, 0, 8, 2);

    // Columns 2-7 - Download List and file sharing
    layout->addWidget(fileList, 1, 2, 7, 6);
    layout->addWidget(shareFileButton, 0, 2, 1, 3);
    layout->addWidget(deleteFileButton, 0, 5, 1, 3);

    // Column 8 - Peers
    layout->addWidget(textNeigh, 0, 8, 1, 2);
    layout->addWidget(chatList, 1, 8, 7, 2);

	setLayout(layout);
	
	// Misc additions
	QIcon icon;
	icon.addFile("./tux.png");
	setWindowIcon(icon);
	QWidget::setWindowTitle(host);

	// Correct way to set focus to textline
    setTabOrder(textNeigh, chatList);
    setTabOrder(chatList, shareFileButton);
    setTabOrder(shareFileButton, textNeigh);

    // Database
    db = new Database(this);
    if (!db->openDB()){
        qDebug() << "Failed to open database. Dying";
        exit(1);
    }
    else{
        qDebug() << "Database opened successfully.";
        if (!(db->setUpDataTable())) exit(1);
        if (!(db->setUpFileTable())) exit(1);
    }

	// Add self to status
	addToStatus(host, 1);

    // Connect buttons and 'return' presses
	connect(textNeigh, SIGNAL(returnPressed()), this, SLOT(newNeighInput()));
    connect(shareFileButton, SIGNAL(clicked()), this, SLOT(showShareFileDialog()));
    connect(deleteFileButton, SIGNAL(clicked()), this, SLOT(deleteSelectedFiles()));

    connect(shareFileDialog, SIGNAL(filesSelected(QStringList)), this, SLOT(shareFiles(QStringList)));

    // Send status to random neighbor every 2 seconds
    connect(&statusTimer, SIGNAL(timeout()), this, SLOT(sendStatus()));
    statusTimer.start(STATUS_PERIOD);

    // Send route rumor message to random neighbor every 60 seconds
    connect(&routeTimer, SIGNAL(timeout()), this, SLOT(sendRoute()));
    routeTimer.start(ROUTE_PERIOD);
}

// #### SHARED FILE FUNCTIONS ####

void ChatDialog::shareFiles(QStringList files)
{
    for(int i = 0; i < files.size(); ++i){
        buildSharedFile(files.at(i));
    }
}

// Gets file info, builds the SharedFile object, builds the merkle tree in the database,
// and puts the file in database.
void ChatDialog::buildSharedFile(QString filePath)
{
    QFile file(filePath);
    QFileInfo fileInfo(file);
    QString name;
    quint64 size;
    SharedFile *sharedFile;
    QByteArray hashHead;
    int fileId;

    if (!file.open(QIODevice::ReadOnly)){
        qDebug() << "Error reading file" << filePath;
        return;
    }

    QDataStream in(&file);

    // File name
    name = fileInfo.fileName();
    // File size in bytes
    size = fileInfo.size();

    hashHead = buildMerkleTree(in);

    // Insert to database
    fileId = db->insertFile(name, size, hashHead);

    sharedFile = new SharedFile(name, size, hashHead, fileId);

    // Put on GUI list
    putOnFileList(FILE_COMPLETE, name, size, fileId);
    // Put on list
    sharedFiles.insert(sharedFile->id, sharedFile);
}

// Deletes file selected in the files list
void ChatDialog::deleteSelectedFiles()
{
    if (!(fileList->selectionModel()->hasSelection()))
        return;

    int row = fileList->selectionModel()->selectedRows().first().row();
    FileListItem *selected = (FileListItem*) fileList->itemAt(row,0);

    // Clean up file name, in case it has directories
    quint32 id = selected->id;

    if (fileList->item(row, STATUS_COLUMN)->text() == "Sharing"){
        db->deleteFile(id);
        sharedFiles.remove(id);
    }
    for (int col = 0; col < NCOLUMNS; ++col)
        delete fileList->item(row, col);
    fileList->removeRow(row);
}

// Loads files already in DB at startup
void ChatDialog::loadSharedFromDB(QString fileName, quint64 size, QByteArray hashHead, quint32 id)
{
    SharedFile *sharedFile = new SharedFile(fileName, size, hashHead, id);
    putOnFileList(FILE_COMPLETE, fileName, size, id);
    sharedFiles.insert(sharedFile->id, sharedFile);
}

// ## MERKLE TREE BUILDING FUNCTIONS ####

// Fills Q0. The first queue in the merkle tree, building algorithm. Q0 is filled
// with hashes of blocks of the shared file.
void ChatDialog::fillQ0(QDataStream &in, QQueue<QByteArray> &q, qint64 &position)
{
    char rawBlock[BLOCKSIZE];
    int nRead;
    QByteArray blockHash;
    quint32 i = 0;
    // Need this to be per file
    while (!in.atEnd() && i < HASHESPERBLOCK) {
        nRead = in.readRawData(rawBlock, BLOCKSIZE);

        blockHash = sha1sum(rawBlock, nRead);

        // Add block to map with all data, hash is the key
        //dataMap.insert(blockHash, new Block(true, position, QByteArray(rawBlock, nRead)));
        //qDebug() << "Inserting with pos" << position;
        db->insertData(0, blockHash, QByteArray(rawBlock, nRead), position++);

        q.enqueue(blockHash);
        ++i;
    }
}

// Hashes an entire queue and puts the hash in higher queue.
QByteArray ChatDialog::hashQueue(QQueue<QByteArray> &q)
{
    QByteArray block;
    QByteArray hash;

    QQueue<QByteArray>::iterator it;
    while(!q.isEmpty()){;
        block.append(q.dequeue());
    }
    hash = sha1sum(block);

    //dataMap.insert(hash, new Block(false, 0, block));
    db->insertData(0, hash, block, -1);

    return hash;
}

// Builds the merkle tree of a file. Important: algorithm is complicated
// because tree must be built without loading entire file to memory.
// Uses a queue to store hashes in each level of the tree. Once a queue hash
// HASHESPERBLOCK element, hashQueue() is called.
// Q0 is kept always full. Once Q0 is not full, the tree creation is wrapped up.
QByteArray ChatDialog::buildMerkleTree(QDataStream &in)
{
    // Start actual tree build
    QVector<QQueue<QByteArray> > qs(1);
    quint32 cur_q = 0; // Start at queue 0
    qint64 position = 0;
    fillQ0(in, qs[0], position);

    while (true){
        // Go up, filling and hashing queues
        if((quint32) qs[cur_q].size() == HASHESPERBLOCK){
            QByteArray queueHash = hashQueue(qs[cur_q]); // Also inserts to data map

            // Make sure to keep q0 filled with block hashes
            if (cur_q == 0)
                fillQ0(in, qs[0], position);

            // Create new level queue if needed
            if ((quint32) (qs.size() - 1) == cur_q)
                qs.insert(cur_q + 1, QQueue<QByteArray>());
            qs[cur_q + 1].enqueue(queueHash);
            cur_q++;
        }
        else{
            // Go down, until a queue can be hashed (q0 can always be hashed
            // unless, end of file is readched).
            if (cur_q != 0){
                --cur_q;
                continue;
            }
            // When q0 is empty, ended reading file, wrap up tree.
            else{
                QByteArray hashHead;
                // If currently at top queue and only in queue
                if ((qs[cur_q].size() == 1 && (cur_q == (quint32) qs.size() - 1))){
                    hashHead = qs[cur_q].at(0);
                }
                else{
                    hashHead = hashQueue(qs[cur_q]);
                    while ((cur_q + 1) < (quint32) qs.size()){
                        cur_q++;
                        qs[cur_q].enqueue(hashHead);
                        hashHead = hashQueue(qs[cur_q]);
                    }
                }
                // queueHash is the head of the Merkle tree
                db->execDataInserts();
                return hashHead;
            }
        }
    }
}

// #### DOWNLOAD FUNCTIONS ####

void ChatDialog::startFileDownload(QString fileName, quint64 size, QByteArray hashHead, QList<QString> peers)
{
    FileDownload *download = new FileDownload(fileName, downloadPath, size, hashHead, peers);
    fileDownloads.insert(hashHead, download);

    if (!QDir("downloadPath").exists())
        QDir().mkdir("downloadPath");

    // Create file on disk, set its size
    QFile file(downloadPath + download->fileName);
    file.open(QIODevice::WriteOnly | QIODevice::ReadOnly);
    file.resize(size);
    file.close();

    download->listItem = putOnFileList(FILE_INCOMPLETE, download->fileName, download->size, 0, download->peers.size());

    // Request first layer of merkle tree from random peer
    QString peer = download->freePeers.takeAt(qrand() % download->freePeers.size());
    makeBlockRequest(download, peer, QString(QChar(1)), hashHead);
}

void ChatDialog::makeBlockRequest(FileDownload* download, QString dest, QString priority, QByteArray blockHash)
{
    BlockRequest *newReq = new BlockRequest(download, blockHash, dest, priority, download->peers.size());
    download->pendingReqs.insert(blockHash, newReq);
    hashToFile.insert(blockHash, download);
    connect(newReq, SIGNAL(timeout(BlockRequest*, FileDownload*)),
            this, SLOT(requestTimeout(BlockRequest*, FileDownload*)));

    sendBlockRequest(Peer(), newReq->source, host, myHopLimit, newReq->hash);
    newReq->timer.start();
}

void ChatDialog::requestNth(QString dest, FileDownload *download, quint32 n){
    if (download->blockQ.size() <= (int) n)
           return;

    QString priority = download->blockQ.keys().at(n);
    QByteArray next = download->blockQ.take(priority);

    makeBlockRequest(download, dest, priority, next);
}

// Slot to timeout requests. Connected to timer in BlockRequest object.
void ChatDialog::requestTimeout(BlockRequest *req, FileDownload *download)
{
    qDebug() << "Req timed out" << req->hash.toHex();
    download->pendingReqs.remove(req->hash);
    download->blockQ.insert(req->priority, req->hash);

    // To delayed peers, ask for the fifth in queue
    requestNth(req->source, download, TIMEOUT_NEXT);
    delete req;
}

// Clocks all pending requests. Puts them back in blockQ if timeout.
void ChatDialog::clockRequests(FileDownload *download)
{
    QHash<QByteArray, BlockRequest*>::iterator it = download->pendingReqs.begin();

    while (it != download->pendingReqs.end()){
        (*it)->clock++;
        if((*it)->clock > (*it)->maxClock){
            BlockRequest *req = *it;
            it = download->pendingReqs.erase(it);
            download->blockQ.insert(req->priority, req->hash);
            requestNth(req->source, download, TIMEOUT_NEXT);
            delete req;
        }
        else{
            it++;
        }
    }
}

void ChatDialog::enqueueMetadata(FileDownload* download, BlockRequest* req, QByteArray blockData)
{
    // Put contents in blockQ
    for (int i = 0; (i * HASHSIZE) < blockData.size(); ++i){
        QString newPriority = req->priority + QString(QChar(i + 1));
        QByteArray newData;
        if (blockData.size() < HASHSIZE)
            newData = QByteArray(blockData.data(), blockData.size());
        else
            newData = QByteArray(blockData.data() + (i * HASHSIZE), HASHSIZE);

        download->blockQ.insert(newPriority, newData);
    }
}

void ChatDialog::employPeers(FileDownload* download)
{
    QList<QString>::iterator it = download->freePeers.begin();
    while (it != download->freePeers.end()){
        if (download->blockQ.size() > 0){
            requestNth(*it, download, 0);
            download->freePeers.erase(it++);
        }
        else{
            ++it;
        }
    }
}

void ChatDialog::writeBlock(FileDownload* download, QByteArray data, quint64 idx)
{
    QFile file(downloadPath + download->fileName);
    file.open(QIODevice::WriteOnly | QIODevice::ReadOnly);
    file.seek(idx*BLOCKSIZE);
    file.write(data);
    file.close();
}

void ChatDialog::resolveDownload(FileDownload* download){
    // Clear download table
    int row = -1;
    for(int j = 0; j < fileList->rowCount(); ++j){
        if (fileList->item(j, FILENAME_COLUMN)->text() == download->fileName)
            row = j;
    }
    for (int col = 0; col < NCOLUMNS; ++col)
        delete fileList->item(row, col);
    fileList->removeRow(row);

    delete download;
}

// ##### MESSAGE HANDLING FUNCTIONS #####

void ChatDialog::handleBlockRequest(Peer inPeer, QString dest, QString origin, quint32 hopLimit, QByteArray blockRequest)
{
    // If not for me and forwarding is on
    if (dest != host && forward){
        hopLimit--;
        if (hopLimit > 0)
            sendBlockRequest(inPeer, dest, origin, hopLimit, blockRequest);
        return;
    }
    // For me
    else if (dest == host){
        // Get block using blockrequest as hash
        //Block *replyBlock = dataMap[blockRequest];
        QPair<qint64, QByteArray> replyBlock = db->get(blockRequest);

        // Do nothing if don't have the requested block
        if (replyBlock.first == -2)
            return;

        bool isData = (replyBlock.first >= 0);

        // Uncomment to simulate network behavior
        //usleep(qrand() % 10000); // Send delay
        //if ((qrand() % 30) == 1)  // Drop 1/30 packets
        //    return;

        // Send block back if found
        if (isData)
            qDebug() << "Sending file block #" << replyBlock.first << "to" << inPeer;
        else
            qDebug() << "Sending metadata to" << inPeer;
        sendBlockReply(Peer(), origin, host, myHopLimit, blockRequest, replyBlock.second, isData, replyBlock.first);
    }
}

void ChatDialog::handleBlockReply(QString dest, QString origin, quint32 hopLimit, QByteArray blockReply, QByteArray blockData, bool isData, quint64 idx)
{
    // If not for me and forwarding is on
    if (dest != host && forward){
        hopLimit--;
        if (hopLimit > 0)
            sendBlockReply(Peer(), dest, origin, hopLimit, blockReply, blockData, isData, idx);
        return;
    }

    // For me
    else if (dest == host){
        FileDownload *download = hashToFile[blockReply];
        if (!download) return;
        BlockRequest *req = download->pendingReqs.take(blockReply);
        if (!req) return; // Unexpected reply

        req->timer.stop();

        QByteArray dataHash = sha1sum(blockData);

        // If hash does not match reply
        if (dataHash != blockReply){
            qDebug() << "Data-reply mismatch";
            // Put back in queue
            download->blockQ.insert(req->priority, req->hash);
        }
        else if (isData){
            qDebug() << "Received data block #" << QString::number(idx) << "from" << origin;

            writeBlock(download, blockData, idx);
            updateProgressBar(download, idx);

            if (((idx+1)*BLOCKSIZE) >= download->size){
                resolveDownload(download);
                return;
            }
        }
        else /* is metadata*/ {
            qDebug() << "Received metadata block from" << origin;
            enqueueMetadata(download, req, blockData);
            employPeers(download);
        }
        // Clear request
        hashToFile.remove(blockReply);
        delete req;

        //Clock all other requests
        clockRequests(download);

        // Make new request to peer
        requestNth(origin, download, 0);
    }
}

void ChatDialog::handleSearchRequest(Peer inPeer, QString origin, quint32 budget, QString search)
{
    if (budget <= 0)
        return;

    QStringList searchList = search.split(" ");
    QVariantList myMatchNames;
    QVariantList myMatchIds;
    QVariantList myMatchSizes;

    QStringList::iterator its = searchList.begin();
    QMap<quint32, SharedFile*>::iterator itf = sharedFiles.begin();

    for (; its != searchList.end(); ++its){
        for (; itf != sharedFiles.end(); ++itf){
            // Match found
            if ((*itf)->name.contains(*its, Qt::CaseInsensitive)){
                QVariant name = QVariant((*itf)->name);
                QVariant metaHash = QVariant((*itf)->hashHead);
                QVariant size = QVariant((*itf)->size);

                myMatchNames.append(name);
                myMatchIds.append(metaHash);
                myMatchSizes.append(size);
            }
        }
    }
    // Reply if found something in own files
    if (!myMatchNames.isEmpty()){
        sendSearchReply(Peer(), origin, host, myHopLimit, search, myMatchNames, myMatchIds, myMatchSizes);
    }

    // Forward to other nodes
    budget--;
    // Divide budget evenly and send out
    if (budget > 0){
        int fullLoops;
        int remainder;
        fullLoops = budget / (neighbors.size());
        remainder = budget % (neighbors.size());
        // Iterate over neighbors
        QList<Peer>::iterator itn = neighbors.begin();
        for (; (fullLoops || remainder) && itn != neighbors.end(); ++itn){
            // Skip over neighbor who sent me this request
            if ((*itn) == inPeer)
                continue;
            sendSearchRequest(*itn, origin, search, fullLoops + 1);
            remainder--;
        }
    }
}

void ChatDialog::handleSearchReply(Peer inPeer, QString dest, QString origin, quint32 hopLimit, QString searchReply, QVariantList matchNames, QVariantList matchIds, QVariantList matchSizes)
{
    if (dest != host && forward){
        hopLimit--;
        if (hopLimit > 0)
            sendSearchReply(inPeer, dest, origin, hopLimit, searchReply, matchNames, matchIds, matchSizes);
        return;
    }
    else if (dest == host){
        // Expected search reply
        if (searchReply == searchDialog->currentSearch){
            for (int i = 0; i < matchNames.size() && i < matchIds.size() && i < matchSizes.size(); ++i){
                int rowCount = searchDialog->resultList->rowCount();
                // Check if already have file
                for (int row = 0; row < rowCount; ++row){
                    SearchResult *item = ((SearchResult*) searchDialog->resultList->item(row, 0));
                    if (matchIds.at(i).toByteArray() == item->metaHash){
                        if (!item->peers.contains(origin)){
                            item->peers.insert(origin);
                            searchDialog->resultList->item(row, 1)->setText(QString::number(item->peers.size()));
                        }
                        return;
                    }
                }
                // New file
                QString fileName = matchNames.at(i).toString();
                QByteArray hash = matchIds.at(i).toByteArray();
                quint64 size = matchSizes.at(i).toUInt();
                searchDialog->addResult(fileName, origin, hash, size);
            }
        }
        // Drop unexpected search replies
        else{
            return;
        }
    }
}

void ChatDialog::handleRoute(Peer inPeer, bool isNew, bool isDirect, QString origin, quint32 seqNo, QHostAddress lastIP, quint16 lastPort)
{
    if (isNew){
        // Direct messages have been handled. Indirects are only added if new.
        if (!isDirect)
            putOnTable(origin, Peer(lastIP, lastPort));

        MongMsg *msg = new MongMsg(inPeer, origin, seqNo, NULL);
        putOnArchive(msg);

        // Reply back with status
        sendStatus(inPeer);

        // Always gossip route messages
        broadcastToAll(msg);
    }
    else{
        sendStatus(inPeer);
    }
}

void ChatDialog::handleStatus(Peer inPeer, QVariantMap hisStatus)
{
    // If this is a reply
    if (isOnHold(inPeer)){
        MongMsg *held = getFromHold(inPeer);

        if (held == NULL || held->outPeer.first.isNull())
            return;

        held->stopTimer();

        removeFromHold(inPeer);

        bool sync;

        sync = compareStatus(hisStatus, inPeer);
        if (sync){
            int flip = qrand() % 2;
            if (flip){
                monger(held);
            }
        }
    }
    // Respond to requests.
    else {
        compareStatus(hisStatus, inPeer);
    }
}

// ##### MESSAGE SENDING FUNCTIONS #####

void ChatDialog::sendPacket(QVariantMap packet, Peer outPeer){
    QByteArray *msg = new QByteArray();
    QDataStream stream(msg, QIODevice::WriteOnly);

    stream << packet;

    // Connected to NetSocket::broadcastMsg()
    emit outmsgReady(msg, outPeer.first, outPeer.second);
}

void ChatDialog::sendBlockReply(Peer inPeer, QString dest, QString origin, quint32 hopLimit, QByteArray blockReply, QByteArray blockData, bool isData, quint64 idx)
{
    Peer outPeer;
    QVariantMap *packet;
    QByteArray *msg;
    QDataStream *stream;

    // Get a known peer from table, if not send it to random neighbor
    outPeer = getFromTable(dest);
    if (outPeer.first.isNull())
        outPeer = randomNeighbor(inPeer);

//    qDebug() << "Sending block reply. Sending it to: " << outPeer.first << outPeer.second;
//    qDebug() << "Reply is for: " + dest;
//    qDebug() << "Reply hash is: " + blockReply.toHex();

    packet = new QVariantMap;

    packet->insert("Origin", origin);
    packet->insert("Dest", dest);
    packet->insert("HopLimit", hopLimit);
    packet->insert("BlockReply", blockReply);
    packet->insert("Data", blockData);
    packet->insert("isData", isData);
    packet->insert("Index", idx);

    msg = new QByteArray;
    stream = new QDataStream(msg, QIODevice::WriteOnly);

    (*stream) << (*packet);
    delete stream;

    // Connected to NetSocket::broadcastMsg()
    emit outmsgReady(msg, outPeer.first, outPeer.second);
}

void ChatDialog::sendBlockRequest(Peer inPeer, QString dest, QString origin, quint32 hopLimit, QByteArray blockRequest){
    Peer outPeer;
    QVariantMap packet;

    // Get a known peer from table, if not send it to random neighbor
    outPeer = getFromTable(dest);
    if (outPeer.first.isNull())
        outPeer = randomNeighbor(inPeer);

//    qDebug() << "Sending block request. Sending it to: " << outPeer.first << outPeer.second;
//    qDebug() << "Request is for: " + dest;
//    qDebug() << "Request hash is: " + blockRequest.toHex();

    packet.insert("Origin", origin);
    packet.insert("Dest", dest);
    packet.insert("HopLimit", hopLimit);
    packet.insert("BlockRequest", blockRequest);

    sendPacket(packet, outPeer);
}

void ChatDialog::sendSearchReply(Peer inPeer, QString dest, QString origin, quint32 hopLimit, QString searchReply, QVariantList matchNames, QVariantList matchIds, QVariantList matchSizes)
{
    Peer outPeer;
    QVariantMap packet;

    // Get a known peer from table, if not send it to random neighbor
    if (outPeer.first.isNull())
        outPeer = randomNeighbor(inPeer);

    qDebug() << "Sending search reply to: " << outPeer.first << outPeer.second;
    qDebug() << "Reply is for: " + dest;
    qDebug() << "Reply search term is: " + searchReply;

    packet.insert("Origin", origin);
    packet.insert("Dest", dest);
    packet.insert("HopLimit", hopLimit);
    packet.insert("SearchReply", searchReply);
    packet.insert("MatchNames", matchNames);
    packet.insert("MatchIDs", matchIds);
    packet.insert("MatchSizes", matchSizes);

    sendPacket(packet, outPeer);
}

void ChatDialog::sendSearchRequest(Peer inPeer, QString origin, QString search, quint32 budget)
{
    Peer outPeer;
    QVariantMap packet;

    // Send to random neighbor
    outPeer = randomNeighbor(inPeer);

    //qDebug() << "Sending search request to: " << outPeer.first << outPeer.second;
    //qDebug() << "Search is" << search;
    //qDebug() << "Reply budget is:" << budget;

    packet.insert("Origin", origin);
    packet.insert("Search", search);
    packet.insert("Budget", budget);

    sendPacket(packet, outPeer);
}

void ChatDialog::sendStatus(Peer outPeer)
{
    QVariantMap message;

    if (outPeer.first.isNull())
        outPeer = randomNeighbor();

    message.insert("Want", status);

    //qDebug() << "Sending status to" << outPeer;
    sendPacket(message, outPeer);
}

void ChatDialog::sendRoute(Peer outPeer)
{
    MongMsg *route = new MongMsg(Peer(), host, msgCounter, NULL);
    QVariantMap packet;

    if (outPeer.first.isNull())
        outPeer = randomNeighbor();

    //qDebug() << "Sending route to" << outPeer;

    packet.insert("Origin", host);
    packet.insert("SeqNo", msgCounter);

    addToStatus(host, msgCounter);
    msgCounter++;
    putOnArchive(route);
    //qDebug() << "Sending route to" << outPeer;
    sendPacket(packet, outPeer);
}

void ChatDialog::serializeMsg(MongMsg *msg, Peer destination)
{
    QVariantMap *message = new QVariantMap;

    message->insert("Origin", msg->origin);
    message->insert("SeqNo", msg->seqNo);
    //qDebug() << "Sending message (origin, seqno)" << msg->origin << msg->seqNo;
    if (!msg->chatText.isNull()){
        message->insert("ChatText", msg->chatText);
    }
    // If I am not original sender
    if (msg->inPeer.first.isNull()){
        message->insert("LastIp", msg->inPeer.first.toIPv4Address());
        message->insert("LastPort", msg->inPeer.second);
    }

    QByteArray array;
    QDataStream *stream = new QDataStream(&array, QIODevice::WriteOnly);

    (*stream) << (*message);
    delete stream;
    delete message;

    emit outmsgReady(&array, destination.first, destination.second);
}

void ChatDialog::readNewMsg(QByteArray *bytes, Peer inPeer)
{
    //qDebug() << "Status: " << status;
    QDataStream in(*bytes);
    QVariantMap msgMap;

    if (inPeer.first.isNull())
        return;

    bool isDirect = false, // Is direct route
         isNew = false;    // Is new message (for routes)

    // Read QVariantMap to variables
    in >> msgMap;
    delete bytes;

    // Make sure sender is in neighbors
    addIfNewPeer(inPeer);

    QVariantMap::iterator msg_vars[NKEYTYPES];
    // Set map end as sentinel
    for (int i = 0; i < NKEYTYPES; ++i)
        msg_vars[i] = msgMap.end();

    // Iterate over map
    QVariantMap::iterator it;
    for (it = msgMap.begin(); it != msgMap.end(); ++it){
        switch(keyToType[it.key().toUpper()]){
            case SEQNO:
                msg_vars[SEQNO] = it;
            break;
            case ORIGIN:
                msg_vars[ORIGIN] = it;
            break;
            case LASTIP:
                msg_vars[LASTIP] = it;
            break;
            case LASTPORT:
                msg_vars[LASTPORT] = it;
            break;
            case WANT:
                msg_vars[WANT] = it;
            break;
            case BLOCKREQUEST:
                msg_vars[BLOCKREQUEST] = it;
            break;
            case DEST:
                msg_vars[DEST] = it;
            break;
            case HOPLIMIT:
                msg_vars[HOPLIMIT] = it;
            break;
            case BLOCKREPLY:
                msg_vars[BLOCKREPLY] = it;
            break;
            case DATA:
                msg_vars[DATA] = it;
            break;
            case SEARCH:
                msg_vars[SEARCH] = it;
            break;
            case BUDGET:
                msg_vars[BUDGET] = it;
            break;
            case SEARCHREPLY:
                msg_vars[SEARCHREPLY] = it;
            break;
            case MATCHNAMES:
                msg_vars[MATCHNAMES] = it;
            break;
            case MATCHIDS:
                msg_vars[MATCHIDS] = it;
            break;
            case ISDATA:
                msg_vars[ISDATA] = it;
            break;
            case INDEX:
                msg_vars[INDEX] = it;
            break;
            case MATCHSIZES:
                msg_vars[MATCHSIZES] = it;
            break;
        }
    }

    // Decide msg type and handle it.
    // Code is ugly, but I think it is one of the best ways to do this.
    // Indirect routes
    if (msg_vars[LASTIP] != msgMap.end() && msg_vars[LASTPORT] != msgMap.end() && msg_vars[ORIGIN] != msgMap.end()){
        isDirect = false;
    }
    // Direct routes: always put on table
    else if (msg_vars[ORIGIN] != msgMap.end() && msg_vars[LASTPORT] == msgMap.end() && msg_vars[LASTIP] == msgMap.end()){
        isDirect = true;
        putOnTable((*(msg_vars[ORIGIN])).toString(), inPeer);
    }

    // Route message
    if (msg_vars[SEQNO] != msgMap.end() && msg_vars[ORIGIN] != msgMap.end()){
        //qDebug() << "Got a route message from:" << inPeer;
        isNew = addToStatus((*(msg_vars[ORIGIN])).toString(), (*(msg_vars[SEQNO])).toUInt(), 1);;
        handleRoute(inPeer, isNew, isDirect, (*(msg_vars[ORIGIN])).toString(), (*(msg_vars[SEQNO])).toUInt(), QHostAddress((*(msg_vars[LASTIP])).toInt()), (*(msg_vars[LASTPORT])).toUInt());
        return;
    }
    // Status message
    else if (msg_vars[WANT] != msgMap.end()){
        // qDebug() << "Got a status message from:" << inPeer;
        handleStatus(inPeer, (*(msg_vars[WANT])).toMap());
        return;
    }

    // Block Request
    else if (msg_vars[DEST] != msgMap.end() && msg_vars[ORIGIN] != msgMap.end() && msg_vars[HOPLIMIT] != msgMap.end() && msg_vars[BLOCKREQUEST] != msgMap.end()){
        //qDebug() << "Got a block request from:" << inPeer.first << inPeer.second;
        //qDebug() << "Block request is:" << (*(msg_vars[BLOCKREQUEST])).toByteArray().toHex();
        handleBlockRequest(inPeer, (*(msg_vars[DEST])).toString(), (*(msg_vars[ORIGIN])).toString(), (*(msg_vars[HOPLIMIT])).toUInt(),
                           (*(msg_vars[BLOCKREQUEST])).toByteArray());
        return;
    }

    // Block Reply
    else if (msg_vars[DEST] != msgMap.end() && msg_vars[ORIGIN] != msgMap.end() && msg_vars[HOPLIMIT] != msgMap.end() && msg_vars[BLOCKREPLY] != msgMap.end() && msg_vars[DATA] != msgMap.end()
            && msg_vars[ISDATA] != msgMap.end() && msg_vars[INDEX] != msgMap.end()){
        //qDebug() << "Got a block reply from:" << inPeer.first << inPeer.second;
        //qDebug() << "Block reply is:" << (*(msg_vars[BLOCKREPLY])).toByteArray().toHex();
        //qDebug() << "Reply content is:" << (*(msg_vars[DATA])).toByteArray().toHex();
        handleBlockReply((*(msg_vars[DEST])).toString(), (*(msg_vars[ORIGIN])).toString(), (*(msg_vars[HOPLIMIT])).toUInt(), (*(msg_vars[BLOCKREPLY])).toByteArray(),
                         (*(msg_vars[DATA])).toByteArray(), (*(msg_vars[ISDATA])).toBool(), (*(msg_vars[INDEX])).toUInt());
        return;
    }

    // Search Request
    else if (msg_vars[ORIGIN] != msgMap.end() && msg_vars[BUDGET] != msgMap.end() && msg_vars[SEARCH] != msgMap.end()){
        //qDebug() << "Got a search request from:" << inPeer.first << inPeer.second;
        //qDebug() << "Search request is:" << (*(msg_vars[SEARCH])).toString();
        handleSearchRequest(inPeer, (*(msg_vars[ORIGIN])).toString(), (*(msg_vars[BUDGET])).toUInt(), (*(msg_vars[SEARCH])).toString());
        return;
    }

    // Search Reply
    else if (msg_vars[DEST] != msgMap.end() && msg_vars[ORIGIN] != msgMap.end()  && msg_vars[HOPLIMIT] != msgMap.end() && msg_vars[SEARCHREPLY] != msgMap.end() && msg_vars[MATCHNAMES] != msgMap.end()
            && msg_vars[MATCHIDS] != msgMap.end() && msg_vars[MATCHSIZES] != msgMap.end()){
        //qDebug() << "Got a search reply from:" << inPeer.first << inPeer.second;
        //qDebug() << "Search reply is:" << (*(msg_vars[SEARCHREPLY])).toString();
        handleSearchReply(inPeer, (*(msg_vars[DEST])).toString(), (*(msg_vars[ORIGIN])).toString(), (*(msg_vars[HOPLIMIT])).toUInt(), (*(msg_vars[SEARCHREPLY])).toString(),
                          (*(msg_vars[MATCHNAMES])).toList(), (*(msg_vars[MATCHIDS])).toList(), (*(msg_vars[MATCHSIZES])).toList());
        return;
    }
}

// #### RUMOR MONGERING FUNCTIONS #####

void ChatDialog::monger(MongMsg *msg)
{
    // Send to random neighbor, except origin or last peer which it was sent to
    Peer outPeer = randomNeighbor(msg->inPeer, msg->outPeer);

    // Do not send to invalid port (see randomNeighbor())
    if (outPeer.first.isNull())
        return;

    // Remember where message was sent
    msg->outPeer = outPeer;

    serializeMsg(msg, outPeer);

    msg->setTimer(MONG_TIMEOUT);

    connect(msg, SIGNAL(timeout(MongMsg*)), this, SLOT(rumorTimeout(MongMsg*)));

    // Add to list of people I started mongering with
    putOnHold(msg);
}

void ChatDialog::rumorTimeout(MongMsg* msg)
{
    removeFromHold(msg->outPeer);

    // Keep mongering if no response
    monger(msg);
}

// Returns true if statuses are the same, returns false otherwise.
// If has message for other, sends it. If other has message
// send status.
bool ChatDialog::compareStatus(QVariantMap hisStatus, Peer inPeer)
{
    QList<QString> myKeys = status.keys();
    QList<QString> hisKeys = hisStatus.keys();

    QList<QString>::iterator i;
    QList<QString>::iterator j;

    // Add people you just learned of to the want list
    for (i = hisKeys.begin(); i != hisKeys.end(); ++i){
        bool inBoth = false;
        for (j = myKeys.begin(); j != myKeys.end(); ++j){
            if ((*i) == (*j))
                inBoth = true;
        }
        if (!inBoth)
            addToStatus(*i, 1);
    }

    // Check if have something other node doesnt, but only if forwarding is on
    if (forward){
        for (i = hisKeys.begin(); i != hisKeys.end(); ++i){
            QVariant myVar = status[*i];
            QVariant hisVar = hisStatus[*i];

            quint32 myWant = myVar.toUInt();
            quint32 hisWant = hisVar.toUInt();

            // If have message he needs, send it
            if (!myVar.isNull() && myWant > hisWant){
                MongMsg *nextMsg;

                nextMsg = getFromArchive(*i, hisWant);
                // This if check should not be necessary
                if (nextMsg != NULL){
                    serializeMsg(nextMsg, inPeer);
                }
                return false;
            }
        }
    }

    // Check if other node has something this doesn't
    for (i = myKeys.begin(); i != myKeys.end(); ++i){
        QVariant myVar = status[*i];
        QVariant hisVar = hisStatus[*i];

        quint32 myWant = myVar.toUInt();
        quint32 hisWant = hisVar.toUInt();

        if (!hisVar.isNull() && myWant < hisWant){
            //qDebug() << "Sending status because want:" << *i << myWant;
            sendStatus(inPeer);
            return false;
        }
    }
    // qDebug() << "Compared status, synchronized.";
    return true;
}

// Returns true if the message is new (as in it is bigger than my seqNo)
bool ChatDialog::addToStatus(QString origin, quint32 seqNo, int isMsg)
{
    QVariant var = status[origin];

    quint32 want = var.toUInt();

    if (var.isNull()){
        //qDebug() << "Added origin " + origin + " to want for the first time want for him is:" <<  (1 + isMsg);
        status[origin] = seqNo + isMsg;
        // Add to chat list (except myself)
        if (origin != host){
            qDebug() << "Added to chat list" << origin;
            new QListWidgetItem(origin, chatList);
        }
        return true;
    }
    else if (seqNo == want){
        status[origin] = seqNo + 1;
        // qDebug() << "Added to status" << origin << (seqNo + 1) << isMsg;
        return true;
    }
    // This is not a message I have, but I need earlier ones first.
    else if (seqNo > want){
        return false;
    }
    else
        return false;
}

void ChatDialog::broadcastToAll(MongMsg *msg)
{
    QList<Peer >::iterator i;
    for (i = neighbors.begin(); i != neighbors.end(); ++i){
        serializeMsg(msg, *i);
    }
}

// #### NEIGHBOR HANDLING FUNCTIONS ####

void ChatDialog::processNewNeigh(QString input)
{
    QList<QString> inputList = input.split(":");

    if (inputList.size() > 2 || inputList.size() < 2){
        qDebug() << host << "Invalid host/ip:port input";
        return;
    }

    // First element of list should be host, second element should be port
    bool ok = false;
    quint16 newPort = inputList.at(1).toUShort(&ok);

    // Validate port
    if (!ok){
        qDebug() << "Invalid port input";
        return;
    }
    QHostAddress newip = QHostAddress(inputList.at(0));
    if (newip.isNull()){ // Have to resolve
        int hostId = QHostInfo::lookupHost(inputList.at(0), this, SLOT(resolveNeighbor(QHostInfo)));
        Peer newPeer = Peer(newip, newPort);
        unresolvedNeighbors[hostId] = newPeer;
        return;
    }

    Peer newPeer = Peer(newip, newPort);
    neighbors << newPeer;
    qDebug() << "Added a new peer:" << newPeer.first << newPeer.second;
}

void ChatDialog::resolveNeighbor(QHostInfo hostInfo)
{
    // Get unresolved peer
    int id = hostInfo.lookupId();
    Peer unresolved = unresolvedNeighbors[id];

    QList<QHostAddress> addrs = hostInfo.addresses();

    if (hostInfo.error() || addrs.isEmpty()){
        qDebug() << "Could not resolve peer.";
        unresolvedNeighbors.remove(id);
        return;
    }

    // Pick first IP
    unresolved.first = hostInfo.addresses().at(0);

    // Remove from unresolved and put with regular neighbors
    unresolvedNeighbors.remove(id);
    neighbors << unresolved;

    qDebug() << "Added a new peer" << unresolved.first << unresolved.second;
}

// Add neighbors on this node (range of four ports)
void ChatDialog::initNeighbors(QList<quint16>* ports)
{
    //~ qDebug() << "ports" << *ports;
    QList<quint16>::iterator i;
    for (i = ports->begin(); i != ports->end(); ++i){
        Peer newPeer = Peer(QHostAddress::LocalHost, *i);
        neighbors << newPeer;
    }
}

Peer ChatDialog::randomNeighbor(Peer except1, Peer except2)
{
    Peer randomNeigh = neighbors.at(rand() % neighbors.size());

    // In case the only neighbor is an exception
    if ((neighbors.size() == 1 && ((neighbors.at(0) == except1) || (neighbors.at(0) == except2))))
        return Peer(); // Return null
    else if (neighbors.size() == 2 && (((neighbors.at(0) == except1) && (neighbors.at(1) == except2)) ||
            ((neighbors.at(1) == except1) && (neighbors.at(0) == except2))))
        return Peer(); // Return null

    // Do not return the except port
    while ((randomNeigh == except1) || (randomNeigh == except2)){
        randomNeigh = neighbors.at(rand() % neighbors.size());
    }

    return randomNeigh;
}

// #### GUI FUNCTIONS ####

void ChatDialog::updateProgressBar(FileDownload* download, quint64 idx)
{
    download->fileMap.setBit(idx, 1);
    int i = 0;
    while(i < download->fileMap.size() && download->fileMap[i]) i++;
    QProgressBar *progressBar = (QProgressBar*) fileList->cellWidget(download->listItem->row(), PROGRESSBAR_COLUMN);
    if (progressBar->maximum() == 0){
        progressBar->setFormat("%p%");
        progressBar->setMaximum(CEILING(download->size,BLOCKSIZE));
    }
    progressBar->setValue(i);
}

// Sets all the graphical elements for the file list
FileListItem* ChatDialog::putOnFileList(int status, QString fileName, quint64 size, quint32 id, int nPeers)
{
    FileListItem *fileItem = new FileListItem(fileName, nPeers, size, id);
    QProgressBar *progressBar = new QProgressBar();
    QTableWidgetItem *sizeCell = new QTableWidgetItem(sizeInUnits(size));
    QTableWidgetItem *statusCell;

    progressBar->setMinimum(0);

    if (status == FILE_INCOMPLETE){
        statusCell = new QTableWidgetItem(QString("D\\L from " + QString::number(nPeers) + " peers"));
        progressBar->setMaximum(0);
        progressBar->setFormat("Downloading metadata");
    }
    else if (status == FILE_COMPLETE){
        statusCell = new QTableWidgetItem(QString("Sharing"));
        progressBar->setMaximum(1);
        progressBar->setValue(1);
        progressBar->setFormat("Complete");
    }

    Qt::ItemFlags flags = statusCell->flags() & ~((Qt::ItemIsEditable | Qt::ItemIsUserCheckable));
    statusCell->setFlags(flags);
    sizeCell->setFlags(flags);

    fileList->insertRow(fileList->rowCount());
    fileList->setItem(fileList->rowCount() - 1, FILENAME_COLUMN, (QTableWidgetItem*) fileItem);
    fileList->setItem(fileList->rowCount() - 1, STATUS_COLUMN, statusCell);
    fileList->setItem(fileList->rowCount() - 1, SIZE_COLUMN, sizeCell);
    fileList->setCellWidget(fileList->rowCount() - 1, PROGRESSBAR_COLUMN, progressBar);

    return fileItem;
}

// #### UTILITY FUNCTIONS ####

void ChatDialog::initKeyMap(){
    keyToType["SEQNO"] = SEQNO;
    keyToType["ORIGIN"] = ORIGIN;
    keyToType["LASTIP"] = LASTIP;
    keyToType["LASTPORT"] = LASTPORT;
    keyToType["WANT"] = WANT;
    keyToType["BLOCKREQUEST"] = BLOCKREQUEST;
    keyToType["DEST"] = DEST;
    keyToType["HOPLIMIT"] = HOPLIMIT;
    keyToType["BLOCKREPLY"] = BLOCKREPLY;
    keyToType["DATA"] = DATA;
    keyToType["SEARCH"] = SEARCH;
    keyToType["BUDGET"] = BUDGET;
    keyToType["SEARCHREPLY"] = SEARCHREPLY;
    keyToType["MATCHNAMES"] = MATCHNAMES;
    keyToType["MATCHIDS"] = MATCHIDS;
    keyToType["MATCHSIZES"] = MATCHSIZES;
    keyToType["ISDATA"] = ISDATA;
    keyToType["INDEX"] = INDEX;
}

void ChatDialog::putOnArchive(MongMsg* msg)
{
    //qDebug() << "Added to archive" << msg->origin << msg->seqNo << msg->chatText;
    if (msgArchive[msg->origin].isEmpty()){
        QMap<quint32, MongMsg*> origMap;
        origMap[msg->seqNo] = msg;
        msgArchive[msg->origin] = origMap;
    }
    else
        msgArchive[msg->origin][msg->seqNo] = msg;
}

MongMsg* ChatDialog::getFromArchive(QString origin, quint32 seqNo)
{
    if (msgArchive[origin].isEmpty())
        return NULL;
    else
        return msgArchive[origin][seqNo];
}

void ChatDialog::putOnTable(QString origin, Peer pair)
{
	routeTable.insert(origin, pair);
}

Peer ChatDialog::getFromTable(QString origin)
{
	if (!routeTable.contains(origin)){
        Peer empty;
		return empty;
	}
	return routeTable[origin];
}

void ChatDialog::removeFromTable(QString origin)
{
	routeTable.remove(origin);
}

void ChatDialog::putOnHold(MongMsg* msg)
{
	onHold << msg;
}

MongMsg* ChatDialog::getFromHold(Peer get)
{
	QList<MongMsg*>::iterator i;

	for (i = onHold.begin(); i != onHold.end(); ++i){
        if (get == (*i)->outPeer)
			return *i;
	}
    return NULL;
}

void ChatDialog::removeFromHold(Peer rem)
{
	QList<MongMsg*>::iterator i;

	for (i = onHold.begin(); i != onHold.end(); ++i){
        if (rem == (*i)->outPeer){
			i = onHold.erase(i);
			break;
		}
	}
}

bool ChatDialog::isOnHold(Peer check)
{
    QList<MongMsg*>::iterator i;

    for (i = onHold.begin(); i != onHold.end(); ++i){
        if (check == (*i)->outPeer)
            return true;
    }
    return false;
}

void ChatDialog::addIfNewPeer(Peer candidate)
{
    QList<Peer >::iterator i;

	// Dont add self!
    if (candidate.first == QHostAddress::LocalHost && candidate.second == port)
		return;

	for (i = neighbors.begin(); i != neighbors.end(); ++i){
        if (candidate == (*i))
			return;
	}
	// If not equal to anything in list
    neighbors << candidate;
    qDebug() << "Added new peer:" << candidate;
}

void ChatDialog::setPort(int p)
{
    port = p;
    qDebug() << "UPD Port:" << port;

    QWidget::setWindowTitle(host + " at Port: " + QString::number(port));
}

void ChatDialog::setForwarding(bool set)
{
	forward = set;
}

void ChatDialog::showShareFileDialog()
{
    shareFileDialog->show();
    shareFileDialog->raise();
}

void ChatDialog::newNeighInput()
{
	QString input = textNeigh->text();
	processNewNeigh(input);
	textNeigh->clear();
}

QString sizeInUnits(quint64 sizeInt){
    double size = (double) sizeInt;
    QStringList units;
    int i = 0;
    units << " B" << " KiB" << " MiB" << " GiB" << " TiB" << " PiB";
    while (size >= 1024){
        size /= 1024;
        i++;
    }
    return QString::number(size, 'f', 2) + units.at(i);
}
