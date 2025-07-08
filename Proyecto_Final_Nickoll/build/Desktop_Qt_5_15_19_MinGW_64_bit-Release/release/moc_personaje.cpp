/****************************************************************************
** Meta object code from reading C++ file 'personaje.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.15.19)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include <memory>
#include "../../../personaje.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'personaje.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.15.19. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct qt_meta_stringdata_Personaje_t {
    QByteArrayData data[3];
    char stringdata0[31];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Personaje_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Personaje_t qt_meta_stringdata_Personaje = {
    {
QT_MOC_LITERAL(0, 0, 9), // "Personaje"
QT_MOC_LITERAL(1, 10, 19), // "actualizarAnimacion"
QT_MOC_LITERAL(2, 30, 0) // ""

    },
    "Personaje\0actualizarAnimacion\0"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Personaje[] = {

 // content:
       8,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    0,   19,    2, 0x08 /* Private */,

 // slots: parameters
    QMetaType::Void,

       0        // eod
};

void Personaje::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        auto *_t = static_cast<Personaje *>(_o);
        (void)_t;
        switch (_id) {
        case 0: _t->actualizarAnimacion(); break;
        default: ;
        }
    }
    (void)_a;
}

QT_INIT_METAOBJECT const QMetaObject Personaje::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_meta_stringdata_Personaje.data,
    qt_meta_data_Personaje,
    qt_static_metacall,
    nullptr,
    nullptr
} };


const QMetaObject *Personaje::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Personaje::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_Personaje.stringdata0))
        return static_cast<void*>(this);
    if (!strcmp(_clname, "QGraphicsPixmapItem"))
        return static_cast< QGraphicsPixmapItem*>(this);
    return QObject::qt_metacast(_clname);
}

int Personaje::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 1;
    }
    return _id;
}
QT_WARNING_POP
QT_END_MOC_NAMESPACE
