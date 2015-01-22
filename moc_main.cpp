/****************************************************************************
** Meta object code from reading C++ file 'main.hh'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.6)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "main.hh"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'main.hh' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.6. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TextEdit[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      10,    9,    9,    9, 0x05,

       0        // eod
};

static const char qt_meta_stringdata_TextEdit[] = {
    "TextEdit\0\0returnPressed()\0"
};

void TextEdit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        TextEdit *_t = static_cast<TextEdit *>(_o);
        switch (_id) {
        case 0: _t->returnPressed(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData TextEdit::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TextEdit::staticMetaObject = {
    { &QPlainTextEdit::staticMetaObject, qt_meta_stringdata_TextEdit,
      qt_meta_data_TextEdit, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TextEdit::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TextEdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TextEdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TextEdit))
        return static_cast<void*>(const_cast< TextEdit*>(this));
    return QPlainTextEdit::qt_metacast(_clname);
}

int TextEdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPlainTextEdit::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}

// SIGNAL 0
void TextEdit::returnPressed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
static const uint qt_meta_data_MongMsg[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
       9,    8,    8,    8, 0x05,

 // slots: signature, parameters, type, tag, flags
      27,    8,    8,    8, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_MongMsg[] = {
    "MongMsg\0\0timeout(MongMsg*)\0sendTimeout()\0"
};

void MongMsg::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MongMsg *_t = static_cast<MongMsg *>(_o);
        switch (_id) {
        case 0: _t->timeout((*reinterpret_cast< MongMsg*(*)>(_a[1]))); break;
        case 1: _t->sendTimeout(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MongMsg::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MongMsg::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_MongMsg,
      qt_meta_data_MongMsg, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MongMsg::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MongMsg::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MongMsg::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MongMsg))
        return static_cast<void*>(const_cast< MongMsg*>(this));
    return QObject::qt_metacast(_clname);
}

int MongMsg::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void MongMsg::timeout(MongMsg * _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_SharedFile[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_SharedFile[] = {
    "SharedFile\0"
};

void SharedFile::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData SharedFile::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SharedFile::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_SharedFile,
      qt_meta_data_SharedFile, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SharedFile::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SharedFile::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SharedFile::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SharedFile))
        return static_cast<void*>(const_cast< SharedFile*>(this));
    return QObject::qt_metacast(_clname);
}

int SharedFile::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_BlockRequest[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   14,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      53,   13,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_BlockRequest[] = {
    "BlockRequest\0\0,\0timeout(BlockRequest*,FileDownload*)\0"
    "sendTimeout()\0"
};

void BlockRequest::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        BlockRequest *_t = static_cast<BlockRequest *>(_o);
        switch (_id) {
        case 0: _t->timeout((*reinterpret_cast< BlockRequest*(*)>(_a[1])),(*reinterpret_cast< FileDownload*(*)>(_a[2]))); break;
        case 1: _t->sendTimeout(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData BlockRequest::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject BlockRequest::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_BlockRequest,
      qt_meta_data_BlockRequest, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &BlockRequest::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *BlockRequest::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *BlockRequest::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BlockRequest))
        return static_cast<void*>(const_cast< BlockRequest*>(this));
    return QObject::qt_metacast(_clname);
}

int BlockRequest::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void BlockRequest::timeout(BlockRequest * _t1, FileDownload * _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_FileDownload[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       0,    0, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

       0        // eod
};

static const char qt_meta_stringdata_FileDownload[] = {
    "FileDownload\0"
};

void FileDownload::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData FileDownload::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FileDownload::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_FileDownload,
      qt_meta_data_FileDownload, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FileDownload::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FileDownload::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FileDownload::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FileDownload))
        return static_cast<void*>(const_cast< FileDownload*>(this));
    return QObject::qt_metacast(_clname);
}

int FileDownload::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_ChatDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      22,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   12,   11,   11, 0x05,

 // slots: signature, parameters, type, tag, flags
      61,   57,   11,   11, 0x0a,
      97,   84,   11,   11, 0x0a,
     126,   11,   11,   11, 0x0a,
     151,  142,   11,   11, 0x0a,
     184,  178,   11,   11, 0x0a,
     219,  209,   11,   11, 0x0a,
     246,  238,   11,   11, 0x0a,
     263,   11,   11,   11, 0x2a,
     276,  238,   11,   11, 0x0a,
     292,   11,   11,   11, 0x2a,
     345,  304,   11,   11, 0x0a,
     463,  403,   11,   11, 0x0a,
     614,  543,   11,   11, 0x0a,
     711,  707,   11,   11, 0x0a,
     774,  759,   11,   11, 0x0a,
     803,   11,   11,   11, 0x0a,
     831,  825,   11,   11, 0x0a,
     884,  855,   11,   11, 0x0a,
     958,  945,   11,   11, 0x0a,
    1028, 1002,   11,   11, 0x0a,
    1081,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ChatDialog[] = {
    "ChatDialog\0\0,,\0outmsgReady(QByteArray*,QHostAddress,int)\0"
    "msg\0rumorTimeout(MongMsg*)\0bytes,inpeer\0"
    "readNewMsg(QByteArray*,Peer)\0"
    "newNeighInput()\0hostInfo\0"
    "resolveNeighbor(QHostInfo)\0input\0"
    "processNewNeigh(QString)\0candidate\0"
    "addIfNewPeer(Peer)\0outPeer\0sendStatus(Peer)\0"
    "sendStatus()\0sendRoute(Peer)\0sendRoute()\0"
    "inPeer,dest,origin,hopLimit,blockRequest\0"
    "sendBlockRequest(Peer,QString,QString,quint32,QByteArray)\0"
    "inPeer,dest,origin,hopLimit,blockReply,blockData,isData,idx\0"
    "sendBlockReply(Peer,QString,QString,quint32,QByteArray,QByteArray,bool"
    ",quint64)\0"
    "inPeer,dest,origin,hopLimit,searchReply,matchNames,matchIds,matchSizes\0"
    "sendSearchReply(Peer,QString,QString,quint32,QString,QVariantList,QVar"
    "iantList,QVariantList)\0"
    ",,,\0sendSearchRequest(Peer,QString,QString,quint32)\0"
    "packet,outPeer\0sendPacket(QVariantMap,Peer)\0"
    "showShareFileDialog()\0files\0"
    "shareFiles(QStringList)\0"
    "fileName,size,hashHead,peers\0"
    "startFileDownload(QString,quint64,QByteArray,QList<QString>)\0"
    "req,download\0requestTimeout(BlockRequest*,FileDownload*)\0"
    "filename,size,hashHead,id\0"
    "loadSharedFromDB(QString,quint64,QByteArray,quint32)\0"
    "deleteSelectedFiles()\0"
};

void ChatDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ChatDialog *_t = static_cast<ChatDialog *>(_o);
        switch (_id) {
        case 0: _t->outmsgReady((*reinterpret_cast< QByteArray*(*)>(_a[1])),(*reinterpret_cast< QHostAddress(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 1: _t->rumorTimeout((*reinterpret_cast< MongMsg*(*)>(_a[1]))); break;
        case 2: _t->readNewMsg((*reinterpret_cast< QByteArray*(*)>(_a[1])),(*reinterpret_cast< Peer(*)>(_a[2]))); break;
        case 3: _t->newNeighInput(); break;
        case 4: _t->resolveNeighbor((*reinterpret_cast< QHostInfo(*)>(_a[1]))); break;
        case 5: _t->processNewNeigh((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 6: _t->addIfNewPeer((*reinterpret_cast< Peer(*)>(_a[1]))); break;
        case 7: _t->sendStatus((*reinterpret_cast< Peer(*)>(_a[1]))); break;
        case 8: _t->sendStatus(); break;
        case 9: _t->sendRoute((*reinterpret_cast< Peer(*)>(_a[1]))); break;
        case 10: _t->sendRoute(); break;
        case 11: _t->sendBlockRequest((*reinterpret_cast< Peer(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< quint32(*)>(_a[4])),(*reinterpret_cast< QByteArray(*)>(_a[5]))); break;
        case 12: _t->sendBlockReply((*reinterpret_cast< Peer(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< quint32(*)>(_a[4])),(*reinterpret_cast< QByteArray(*)>(_a[5])),(*reinterpret_cast< QByteArray(*)>(_a[6])),(*reinterpret_cast< bool(*)>(_a[7])),(*reinterpret_cast< quint64(*)>(_a[8]))); break;
        case 13: _t->sendSearchReply((*reinterpret_cast< Peer(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< quint32(*)>(_a[4])),(*reinterpret_cast< QString(*)>(_a[5])),(*reinterpret_cast< QVariantList(*)>(_a[6])),(*reinterpret_cast< QVariantList(*)>(_a[7])),(*reinterpret_cast< QVariantList(*)>(_a[8]))); break;
        case 14: _t->sendSearchRequest((*reinterpret_cast< Peer(*)>(_a[1])),(*reinterpret_cast< QString(*)>(_a[2])),(*reinterpret_cast< QString(*)>(_a[3])),(*reinterpret_cast< quint32(*)>(_a[4]))); break;
        case 15: _t->sendPacket((*reinterpret_cast< QVariantMap(*)>(_a[1])),(*reinterpret_cast< Peer(*)>(_a[2]))); break;
        case 16: _t->showShareFileDialog(); break;
        case 17: _t->shareFiles((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 18: _t->startFileDownload((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< quint64(*)>(_a[2])),(*reinterpret_cast< QByteArray(*)>(_a[3])),(*reinterpret_cast< QList<QString>(*)>(_a[4]))); break;
        case 19: _t->requestTimeout((*reinterpret_cast< BlockRequest*(*)>(_a[1])),(*reinterpret_cast< FileDownload*(*)>(_a[2]))); break;
        case 20: _t->loadSharedFromDB((*reinterpret_cast< QString(*)>(_a[1])),(*reinterpret_cast< quint64(*)>(_a[2])),(*reinterpret_cast< QByteArray(*)>(_a[3])),(*reinterpret_cast< quint32(*)>(_a[4]))); break;
        case 21: _t->deleteSelectedFiles(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ChatDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ChatDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_ChatDialog,
      qt_meta_data_ChatDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ChatDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ChatDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ChatDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ChatDialog))
        return static_cast<void*>(const_cast< ChatDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int ChatDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 22)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 22;
    }
    return _id;
}

// SIGNAL 0
void ChatDialog::outmsgReady(QByteArray * _t1, QHostAddress _t2, int _t3)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_SearchDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      39,   13,   13,   13, 0x0a,
      48,   13,   13,   13, 0x0a,
      63,   13,   13,   13, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SearchDialog[] = {
    "SearchDialog\0\0searchRequested(QString)\0"
    "search()\0expandSearch()\0startDownload()\0"
};

void SearchDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SearchDialog *_t = static_cast<SearchDialog *>(_o);
        switch (_id) {
        case 0: _t->searchRequested((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->search(); break;
        case 2: _t->expandSearch(); break;
        case 3: _t->startDownload(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SearchDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SearchDialog::staticMetaObject = {
    { &QGroupBox::staticMetaObject, qt_meta_stringdata_SearchDialog,
      qt_meta_data_SearchDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SearchDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SearchDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SearchDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SearchDialog))
        return static_cast<void*>(const_cast< SearchDialog*>(this));
    return QGroupBox::qt_metacast(_clname);
}

int SearchDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QGroupBox::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void SearchDialog::searchRequested(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
static const uint qt_meta_data_NetSocket[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   11,   10,   10, 0x05,
      43,   10,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      63,   60,   10,   10, 0x0a,
     106,   10,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_NetSocket[] = {
    "NetSocket\0\0,\0msgReceived(QByteArray*,Peer)\0"
    "gotNewPeer(Peer)\0,,\0"
    "broadcastMsg(QByteArray*,QHostAddress,int)\0"
    "receiveMsg()\0"
};

void NetSocket::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        NetSocket *_t = static_cast<NetSocket *>(_o);
        switch (_id) {
        case 0: _t->msgReceived((*reinterpret_cast< QByteArray*(*)>(_a[1])),(*reinterpret_cast< Peer(*)>(_a[2]))); break;
        case 1: _t->gotNewPeer((*reinterpret_cast< Peer(*)>(_a[1]))); break;
        case 2: _t->broadcastMsg((*reinterpret_cast< QByteArray*(*)>(_a[1])),(*reinterpret_cast< QHostAddress(*)>(_a[2])),(*reinterpret_cast< int(*)>(_a[3]))); break;
        case 3: _t->receiveMsg(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData NetSocket::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NetSocket::staticMetaObject = {
    { &QUdpSocket::staticMetaObject, qt_meta_stringdata_NetSocket,
      qt_meta_data_NetSocket, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NetSocket::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NetSocket::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NetSocket::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NetSocket))
        return static_cast<void*>(const_cast< NetSocket*>(this));
    return QUdpSocket::qt_metacast(_clname);
}

int NetSocket::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QUdpSocket::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}

// SIGNAL 0
void NetSocket::msgReceived(QByteArray * _t1, Peer _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void NetSocket::gotNewPeer(Peer _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
