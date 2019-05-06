/****************************************************************************
** Meta object code from reading C++ file 'gameui.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.12.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../gameui.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'gameui.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.12.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_GameUI_t {
    QByteArrayData data[15];
    char stringdata0[269];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_GameUI_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_GameUI_t qt_meta_stringdata_GameUI = {
    {
QT_MOC_LITERAL(0, 0, 6), // "GameUI"
QT_MOC_LITERAL(1, 7, 22), // "start_pushButton_start"
QT_MOC_LITERAL(2, 30, 0), // ""
QT_MOC_LITERAL(3, 31, 25), // "start_pushButton_tutorial"
QT_MOC_LITERAL(4, 57, 22), // "end_pushButton_restart"
QT_MOC_LITERAL(5, 80, 14), // "checkBox_sound"
QT_MOC_LITERAL(6, 95, 5), // "value"
QT_MOC_LITERAL(7, 101, 15), // "checkBox_gizmos"
QT_MOC_LITERAL(8, 117, 18), // "checkBox_wireFrame"
QT_MOC_LITERAL(9, 136, 21), // "checkBox_fbxRendering"
QT_MOC_LITERAL(10, 158, 24), // "checkBox_playerRendering"
QT_MOC_LITERAL(11, 183, 21), // "checkBox_currentStage"
QT_MOC_LITERAL(12, 205, 15), // "pushButton_text"
QT_MOC_LITERAL(13, 221, 21), // "start_pushButton_info"
QT_MOC_LITERAL(14, 243, 25) // "start_pushButton_infoExit"

    },
    "GameUI\0start_pushButton_start\0\0"
    "start_pushButton_tutorial\0"
    "end_pushButton_restart\0checkBox_sound\0"
    "value\0checkBox_gizmos\0checkBox_wireFrame\0"
    "checkBox_fbxRendering\0checkBox_playerRendering\0"
    "checkBox_currentStage\0pushButton_text\0"
    "start_pushButton_info\0start_pushButton_infoExit"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_GameUI[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   74,    2, 0x08 /* Private */,
       3,    0,   75,    2, 0x08 /* Private */,
       4,    0,   76,    2, 0x08 /* Private */,
       5,    1,   77,    2, 0x08 /* Private */,
       7,    1,   80,    2, 0x08 /* Private */,
       8,    1,   83,    2, 0x08 /* Private */,
       9,    1,   86,    2, 0x08 /* Private */,
      10,    1,   89,    2, 0x08 /* Private */,
      11,    1,   92,    2, 0x08 /* Private */,
      12,    0,   95,    2, 0x08 /* Private */,
      13,    0,   96,    2, 0x08 /* Private */,
      14,    0,   97,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void, QMetaType::Bool,    6,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

void GameUI::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<GameUI *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->start_pushButton_start(); break;
        case 1: _t->start_pushButton_tutorial(); break;
        case 2: _t->end_pushButton_restart(); break;
        case 3: _t->checkBox_sound((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->checkBox_gizmos((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->checkBox_wireFrame((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->checkBox_fbxRendering((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->checkBox_playerRendering((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->checkBox_currentStage((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 9: _t->pushButton_text(); break;
        case 10: _t->start_pushButton_info(); break;
        case 11: _t->start_pushButton_infoExit(); break;
        default: ;
        }
    }
}

QT_INIT_METAOBJECT const QMetaObject GameUI::staticMetaObject = { {
    &QMainWindow::staticMetaObject,
    qt_meta_stringdata_GameUI.data,
    qt_meta_data_GameUI,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *GameUI::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *GameUI::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_GameUI.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int GameUI::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 12;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
