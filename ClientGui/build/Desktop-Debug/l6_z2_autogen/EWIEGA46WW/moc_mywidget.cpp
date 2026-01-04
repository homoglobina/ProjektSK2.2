/****************************************************************************
** Meta object code from reading C++ file 'mywidget.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.4.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../../mywidget.h"
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'mywidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.4.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
namespace {
struct qt_meta_stringdata_MyWidget_t {
    uint offsetsAndSizes[38];
    char stringdata0[9];
    char stringdata1[11];
    char stringdata2[1];
    char stringdata3[11];
    char stringdata4[20];
    char stringdata5[22];
    char stringdata6[23];
    char stringdata7[25];
    char stringdata8[17];
    char stringdata9[5];
    char stringdata10[18];
    char stringdata11[21];
    char stringdata12[21];
    char stringdata13[12];
    char stringdata14[15];
    char stringdata15[12];
    char stringdata16[16];
    char stringdata17[29];
    char stringdata18[12];
};
#define QT_MOC_LITERAL(ofs, len) \
    uint(sizeof(qt_meta_stringdata_MyWidget_t::offsetsAndSizes) + ofs), len 
Q_CONSTINIT static const qt_meta_stringdata_MyWidget_t qt_meta_stringdata_MyWidget = {
    {
        QT_MOC_LITERAL(0, 8),  // "MyWidget"
        QT_MOC_LITERAL(9, 10),  // "joinBtnHit"
        QT_MOC_LITERAL(20, 0),  // ""
        QT_MOC_LITERAL(21, 10),  // "sendBtnHit"
        QT_MOC_LITERAL(32, 19),  // "onRefreshBtnClicked"
        QT_MOC_LITERAL(52, 21),  // "onJoinLobbyBtnClicked"
        QT_MOC_LITERAL(74, 22),  // "onDisconnectBtnClicked"
        QT_MOC_LITERAL(97, 24),  // "onLobbyItemDoubleClicked"
        QT_MOC_LITERAL(122, 16),  // "QListWidgetItem*"
        QT_MOC_LITERAL(139, 4),  // "item"
        QT_MOC_LITERAL(144, 17),  // "onLeaveBtnClicked"
        QT_MOC_LITERAL(162, 20),  // "onStartButtonClicked"
        QT_MOC_LITERAL(183, 20),  // "onSendAnswersClicked"
        QT_MOC_LITERAL(204, 11),  // "onConnected"
        QT_MOC_LITERAL(216, 14),  // "onDisconnected"
        QT_MOC_LITERAL(231, 11),  // "onReadyRead"
        QT_MOC_LITERAL(243, 15),  // "onErrorOccurred"
        QT_MOC_LITERAL(259, 28),  // "QAbstractSocket::SocketError"
        QT_MOC_LITERAL(288, 11)   // "socketError"
    },
    "MyWidget",
    "joinBtnHit",
    "",
    "sendBtnHit",
    "onRefreshBtnClicked",
    "onJoinLobbyBtnClicked",
    "onDisconnectBtnClicked",
    "onLobbyItemDoubleClicked",
    "QListWidgetItem*",
    "item",
    "onLeaveBtnClicked",
    "onStartButtonClicked",
    "onSendAnswersClicked",
    "onConnected",
    "onDisconnected",
    "onReadyRead",
    "onErrorOccurred",
    "QAbstractSocket::SocketError",
    "socketError"
};
#undef QT_MOC_LITERAL
} // unnamed namespace

Q_CONSTINIT static const uint qt_meta_data_MyWidget[] = {

 // content:
      10,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   92,    2, 0x08,    1 /* Private */,
       3,    0,   93,    2, 0x08,    2 /* Private */,
       4,    0,   94,    2, 0x08,    3 /* Private */,
       5,    0,   95,    2, 0x08,    4 /* Private */,
       6,    0,   96,    2, 0x08,    5 /* Private */,
       7,    1,   97,    2, 0x08,    6 /* Private */,
      10,    0,  100,    2, 0x08,    8 /* Private */,
      11,    0,  101,    2, 0x08,    9 /* Private */,
      12,    0,  102,    2, 0x08,   10 /* Private */,
      13,    0,  103,    2, 0x08,   11 /* Private */,
      14,    0,  104,    2, 0x08,   12 /* Private */,
      15,    0,  105,    2, 0x08,   13 /* Private */,
      16,    1,  106,    2, 0x08,   14 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 17,   18,

       0        // eod
};

Q_CONSTINIT const QMetaObject MyWidget::staticMetaObject = { {
    QMetaObject::SuperData::link<QWidget::staticMetaObject>(),
    qt_meta_stringdata_MyWidget.offsetsAndSizes,
    qt_meta_data_MyWidget,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_stringdata_MyWidget_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MyWidget, std::true_type>,
        // method 'joinBtnHit'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'sendBtnHit'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onRefreshBtnClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onJoinLobbyBtnClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDisconnectBtnClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onLobbyItemDoubleClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QListWidgetItem *, std::false_type>,
        // method 'onLeaveBtnClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onStartButtonClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onSendAnswersClicked'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onConnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onDisconnected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onReadyRead'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onErrorOccurred'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<QAbstractSocket::SocketError, std::false_type>
    >,
    nullptr
} };

void MyWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<MyWidget *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->joinBtnHit(); break;
        case 1: _t->sendBtnHit(); break;
        case 2: _t->onRefreshBtnClicked(); break;
        case 3: _t->onJoinLobbyBtnClicked(); break;
        case 4: _t->onDisconnectBtnClicked(); break;
        case 5: _t->onLobbyItemDoubleClicked((*reinterpret_cast< std::add_pointer_t<QListWidgetItem*>>(_a[1]))); break;
        case 6: _t->onLeaveBtnClicked(); break;
        case 7: _t->onStartButtonClicked(); break;
        case 8: _t->onSendAnswersClicked(); break;
        case 9: _t->onConnected(); break;
        case 10: _t->onDisconnected(); break;
        case 11: _t->onReadyRead(); break;
        case 12: _t->onErrorOccurred((*reinterpret_cast< std::add_pointer_t<QAbstractSocket::SocketError>>(_a[1]))); break;
        default: ;
        }
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 12:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QAbstractSocket::SocketError >(); break;
            }
            break;
        }
    }
}

const QMetaObject *MyWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *MyWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_MyWidget.stringdata0))
        return static_cast<void*>(this);
    return QWidget::qt_metacast(_clname);
}

int MyWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 13)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 13;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
