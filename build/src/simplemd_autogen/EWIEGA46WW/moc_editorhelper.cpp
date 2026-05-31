/****************************************************************************
** Meta object code from reading C++ file 'editorhelper.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/editorhelper.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'editorhelper.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 69
#error "This file was generated using the moc from 6.11.1. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN12EditorHelperE_t {};
} // unnamed namespace

template <> constexpr inline auto EditorHelper::qt_create_metaobjectdata<qt_meta_tag_ZN12EditorHelperE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "EditorHelper",
        "filesDropped",
        "",
        "paths",
        "x",
        "y",
        "refreshBracketMatch",
        "scheduleBracketRefresh",
        "installFileDrop",
        "QQuickWindow*",
        "window",
        "attachBracketMatcher",
        "textArea",
        "QColor",
        "highlightColor",
        "configureSyntaxHighlighting",
        "enabled",
        "heading",
        "marker",
        "code",
        "link",
        "emphasis"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'filesDropped'
        QtMocHelpers::SignalData<void(const QStringList &, qreal, qreal)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QStringList, 3 }, { QMetaType::QReal, 4 }, { QMetaType::QReal, 5 },
        }}),
        // Slot 'refreshBracketMatch'
        QtMocHelpers::SlotData<void()>(6, 2, QMC::AccessPrivate, QMetaType::Void),
        // Slot 'scheduleBracketRefresh'
        QtMocHelpers::SlotData<void()>(7, 2, QMC::AccessPrivate, QMetaType::Void),
        // Method 'installFileDrop'
        QtMocHelpers::MethodData<void(QQuickWindow *)>(8, 2, QMC::AccessPublic, QMetaType::Void, {{
            { 0x80000000 | 9, 10 },
        }}),
        // Method 'attachBracketMatcher'
        QtMocHelpers::MethodData<void(QObject *, const QColor &)>(11, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QObjectStar, 12 }, { 0x80000000 | 13, 14 },
        }}),
        // Method 'configureSyntaxHighlighting'
        QtMocHelpers::MethodData<void(bool, const QColor &, const QColor &, const QColor &, const QColor &, const QColor &)>(15, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 16 }, { 0x80000000 | 13, 17 }, { 0x80000000 | 13, 18 }, { 0x80000000 | 13, 19 },
            { 0x80000000 | 13, 20 }, { 0x80000000 | 13, 21 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<EditorHelper, qt_meta_tag_ZN12EditorHelperE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject EditorHelper::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12EditorHelperE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12EditorHelperE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN12EditorHelperE_t>.metaTypes,
    nullptr
} };

void EditorHelper::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<EditorHelper *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->filesDropped((*reinterpret_cast<std::add_pointer_t<QStringList>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<qreal>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<qreal>>(_a[3]))); break;
        case 1: _t->refreshBracketMatch(); break;
        case 2: _t->scheduleBracketRefresh(); break;
        case 3: _t->installFileDrop((*reinterpret_cast<std::add_pointer_t<QQuickWindow*>>(_a[1]))); break;
        case 4: _t->attachBracketMatcher((*reinterpret_cast<std::add_pointer_t<QObject*>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QColor>>(_a[2]))); break;
        case 5: _t->configureSyntaxHighlighting((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QColor>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QColor>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QColor>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<QColor>>(_a[5])),(*reinterpret_cast<std::add_pointer_t<QColor>>(_a[6]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        switch (_id) {
        default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
        case 3:
            switch (*reinterpret_cast<int*>(_a[1])) {
            default: *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType(); break;
            case 0:
                *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType::fromType< QQuickWindow* >(); break;
            }
            break;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (EditorHelper::*)(const QStringList & , qreal , qreal )>(_a, &EditorHelper::filesDropped, 0))
            return;
    }
}

const QMetaObject *EditorHelper::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *EditorHelper::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN12EditorHelperE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int EditorHelper::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 6)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 6;
    }
    return _id;
}

// SIGNAL 0
void EditorHelper::filesDropped(const QStringList & _t1, qreal _t2, qreal _t3)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1, _t2, _t3);
}
QT_WARNING_POP
