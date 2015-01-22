#include "main.hh"

SearchDialog::SearchDialog(QString title, ChatDialog *prnt) : QGroupBox(title, prnt){
    this->budget = 2;
    this->currentSearch = "";

    timer.setInterval(1000);
    timer.stop();

    searchInput = new QLineEdit(this);
    resultList = new QTableWidget(this);
    searchButton = new QPushButton(QString("Search"), this);
    layout = new QGridLayout(this);
    dad = (ChatDialog*) this->parent();

    // Initialize widget properties
    searchButton->setAutoDefault(false);
    searchButton->setDefault(false);
    searchInput->setPlaceholderText("Search term");

    resultList->verticalHeader()->setVisible(false);
    resultList->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
    resultList->insertColumn(0);
    resultList->insertColumn(1);
    resultList->setSelectionBehavior(QAbstractItemView::SelectRows);
    resultList->setSelectionMode(QAbstractItemView::SingleSelection);

    QStringList headers;
    headers << QString("File") << QString("Peers");
    resultList->setHorizontalHeaderLabels(headers);

    // Row, column, rowspan, colspan
    layout->addWidget(searchInput, 0, 0, 1, 4);
    layout->addWidget(searchButton, 0, 4, 1, 1);
    layout->addWidget(resultList, 1, 0, 4, 5);

    setLayout(layout);

    setTabOrder(searchInput, resultList);
    setTabOrder(resultList, searchButton);

    // Misc additions
    QIcon icon;
    icon.addFile("./tux.png");
    setWindowIcon(icon);
    QWidget::setWindowTitle("Search File");


    // Pressing enter requests whatever is input on either hash or origin input
    // Also clicking the send button
    connect(searchButton, SIGNAL(clicked()), this, SLOT(search()));

    // Timer to expand search
    connect(&timer, SIGNAL(timeout()), this, SLOT(expandSearch()));

    // Clicking on result list
    connect(resultList, SIGNAL(itemDoubleClicked(QTableWidgetItem*)), this, SLOT(startDownload()));
}

void SearchDialog::search(){
    QString searchTerm = searchInput->text();
    budget = 2; // New search, reset budget
    timer.stop();

    if (searchTerm.isEmpty()){
        qDebug() << "Input error: Empty search.";
        currentSearch = "";
        return;
    }

    resultList->setRowCount(0);

    currentSearch = searchTerm;
    dad->sendSearchRequest(Peer(), dad->host, searchTerm, 2);
    timer.start();
}

void SearchDialog::expandSearch(){
    budget *= 2;
    // Never go over a budget of 100
    if (budget >= 100)
        timer.stop();

    dad->sendSearchRequest(Peer(), dad->host, currentSearch, budget);
}

void SearchDialog::startDownload()
{
    int row = resultList->selectionModel()->selectedRows().first().row();
    SearchResult *trueResult = (SearchResult*) resultList->itemAt(row,0);

    // Clean up file name, in case it has directories
    QString realName = trueResult->fileName.split("/").last();

    dad->startFileDownload(realName, trueResult->size, trueResult->metaHash, trueResult->peers.toList());
}

void SearchDialog::addResult(QString fileName, QString origin, QByteArray hash, quint64 size)
{
    SearchResult *result = new SearchResult(fileName, origin, hash, size);

    QTableWidgetItem *peerCell = new QTableWidgetItem(QString::number(1));
    peerCell->setFlags(peerCell->flags() & ~((Qt::ItemIsEditable | Qt::ItemIsUserCheckable)));

    resultList->insertRow(resultList->rowCount());
    int insertAt = resultList->rowCount();
    resultList->setItem(insertAt - 1, 0, (QTableWidgetItem*) result);
    resultList->setItem(insertAt - 1, 1, peerCell);
}

SearchResult::SearchResult(QString fileName, QString peer, QByteArray metaHash, quint64 size) : QTableWidgetItem(fileName)
{
    this->peers = QSet<QString>();
    this->peers.insert(peer);
    this->fileName = fileName;
    this->metaHash = metaHash;
    this->size = size;

    this->setFlags(this->flags() & ~(Qt::ItemIsEditable | Qt::ItemIsUserCheckable));
}
