/****************************************************************************
** Meta object code from reading C++ file 'main.hh'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "main.hh"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'main.hh' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NewBlockThread[] = {

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

static const char qt_meta_stringdata_NewBlockThread[] = {
    "NewBlockThread\0"
};

void NewBlockThread::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    Q_UNUSED(_o);
    Q_UNUSED(_id);
    Q_UNUSED(_c);
    Q_UNUSED(_a);
}

const QMetaObjectExtraData NewBlockThread::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NewBlockThread::staticMetaObject = {
    { &QThread::staticMetaObject, qt_meta_stringdata_NewBlockThread,
      qt_meta_data_NewBlockThread, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NewBlockThread::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NewBlockThread::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NewBlockThread::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NewBlockThread))
        return static_cast<void*>(const_cast< NewBlockThread*>(this));
    return QThread::qt_metacast(_clname);
}

int NewBlockThread::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QThread::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    return _id;
}
static const uint qt_meta_data_PrivateChatWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PrivateChatWindow[] = {
    "PrivateChatWindow\0\0quit()\0"
};

void PrivateChatWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PrivateChatWindow *_t = static_cast<PrivateChatWindow *>(_o);
        switch (_id) {
        case 0: _t->quit(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData PrivateChatWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PrivateChatWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_PrivateChatWindow,
      qt_meta_data_PrivateChatWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PrivateChatWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PrivateChatWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PrivateChatWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PrivateChatWindow))
        return static_cast<void*>(const_cast< PrivateChatWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int PrivateChatWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_PrivateDownloadWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      23,   22,   22,   22, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PrivateDownloadWindow[] = {
    "PrivateDownloadWindow\0\0quit()\0"
};

void PrivateDownloadWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PrivateDownloadWindow *_t = static_cast<PrivateDownloadWindow *>(_o);
        switch (_id) {
        case 0: _t->quit(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData PrivateDownloadWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PrivateDownloadWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_PrivateDownloadWindow,
      qt_meta_data_PrivateDownloadWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PrivateDownloadWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PrivateDownloadWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PrivateDownloadWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PrivateDownloadWindow))
        return static_cast<void*>(const_cast< PrivateDownloadWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int PrivateDownloadWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_PrivateSearchWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      21,   20,   20,   20, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_PrivateSearchWindow[] = {
    "PrivateSearchWindow\0\0quit()\0"
};

void PrivateSearchWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        PrivateSearchWindow *_t = static_cast<PrivateSearchWindow *>(_o);
        switch (_id) {
        case 0: _t->quit(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData PrivateSearchWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject PrivateSearchWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_PrivateSearchWindow,
      qt_meta_data_PrivateSearchWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &PrivateSearchWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *PrivateSearchWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *PrivateSearchWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_PrivateSearchWindow))
        return static_cast<void*>(const_cast< PrivateSearchWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int PrivateSearchWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
static const uint qt_meta_data_ChatDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      12,   11,   11,   11, 0x0a,
      31,   11,   11,   11, 0x0a,
      54,   11,   11,   11, 0x0a,
      68,   11,   11,   11, 0x0a,
      88,   11,   11,   11, 0x0a,
     108,   11,   11,   11, 0x0a,
     133,   11,   11,   11, 0x0a,
     159,   11,   11,   11, 0x0a,
     200,   11,   11,   11, 0x0a,
     221,   11,   11,   11, 0x0a,
     241,   11,   11,   11, 0x0a,
     268,   11,   11,   11, 0x0a,
     307,   11,   11,   11, 0x0a,
     328,   11,   11,   11, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ChatDialog[] = {
    "ChatDialog\0\0gotSubmitPressed()\0"
    "readPendingDatagrams()\0sendInquiry()\0"
    "gotAddPeerPressed()\0lookedUp(QHostInfo)\0"
    "sendPeriodicRouteRumor()\0"
    "sendPeriodicFileInquiry()\0"
    "popUpPrivateChatWindow(QListWidgetItem*)\0"
    "popUpFileSelection()\0popUpFileDownload()\0"
    "popUpPrivateSearchWindow()\0"
    "startDownloadingFile(QListWidgetItem*)\0"
    "updateSearchResult()\0genTrans()\0"
};

void ChatDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ChatDialog *_t = static_cast<ChatDialog *>(_o);
        switch (_id) {
        case 0: _t->gotSubmitPressed(); break;
        case 1: _t->readPendingDatagrams(); break;
        case 2: _t->sendInquiry(); break;
        case 3: _t->gotAddPeerPressed(); break;
        case 4: _t->lookedUp((*reinterpret_cast< const QHostInfo(*)>(_a[1]))); break;
        case 5: _t->sendPeriodicRouteRumor(); break;
        case 6: _t->sendPeriodicFileInquiry(); break;
        case 7: _t->popUpPrivateChatWindow((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 8: _t->popUpFileSelection(); break;
        case 9: _t->popUpFileDownload(); break;
        case 10: _t->popUpPrivateSearchWindow(); break;
        case 11: _t->startDownloadingFile((*reinterpret_cast< QListWidgetItem*(*)>(_a[1]))); break;
        case 12: _t->updateSearchResult(); break;
        case 13: _t->genTrans(); break;
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
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
