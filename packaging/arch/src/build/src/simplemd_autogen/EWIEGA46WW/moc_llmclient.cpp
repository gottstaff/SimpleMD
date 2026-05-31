/****************************************************************************
** Meta object code from reading C++ file 'llmclient.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../../../../src/llmclient.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'llmclient.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN9LlmClientE_t {};
} // unnamed namespace

template <> constexpr inline auto LlmClient::qt_create_metaobjectdata<qt_meta_tag_ZN9LlmClientE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "LlmClient",
        "completed",
        "",
        "text",
        "failed",
        "message",
        "apiBaseUrlChanged",
        "apiKeyChanged",
        "modelChanged",
        "systemPromptChanged",
        "temperatureChanged",
        "maxTokensChanged",
        "busyChanged",
        "lastErrorChanged",
        "requestEdit",
        "instruction",
        "selectedText",
        "beforeCursor",
        "afterCursor",
        "apiBaseUrl",
        "apiKey",
        "model",
        "systemPrompt",
        "temperature",
        "maxTokens",
        "busy",
        "lastError"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'completed'
        QtMocHelpers::SignalData<void(const QString &)>(1, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 3 },
        }}),
        // Signal 'failed'
        QtMocHelpers::SignalData<void(const QString &)>(4, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 5 },
        }}),
        // Signal 'apiBaseUrlChanged'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'apiKeyChanged'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'modelChanged'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'systemPromptChanged'
        QtMocHelpers::SignalData<void()>(9, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'temperatureChanged'
        QtMocHelpers::SignalData<void()>(10, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'maxTokensChanged'
        QtMocHelpers::SignalData<void()>(11, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'busyChanged'
        QtMocHelpers::SignalData<void()>(12, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'lastErrorChanged'
        QtMocHelpers::SignalData<void()>(13, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'requestEdit'
        QtMocHelpers::MethodData<void(const QString &, const QString &, const QString &, const QString &)>(14, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::QString, 15 }, { QMetaType::QString, 16 }, { QMetaType::QString, 17 }, { QMetaType::QString, 18 },
        }}),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'apiBaseUrl'
        QtMocHelpers::PropertyData<QString>(19, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 2),
        // property 'apiKey'
        QtMocHelpers::PropertyData<QString>(20, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 3),
        // property 'model'
        QtMocHelpers::PropertyData<QString>(21, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 4),
        // property 'systemPrompt'
        QtMocHelpers::PropertyData<QString>(22, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 5),
        // property 'temperature'
        QtMocHelpers::PropertyData<double>(23, QMetaType::Double, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 6),
        // property 'maxTokens'
        QtMocHelpers::PropertyData<int>(24, QMetaType::Int, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 7),
        // property 'busy'
        QtMocHelpers::PropertyData<bool>(25, QMetaType::Bool, QMC::DefaultPropertyFlags, 8),
        // property 'lastError'
        QtMocHelpers::PropertyData<QString>(26, QMetaType::QString, QMC::DefaultPropertyFlags, 9),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<LlmClient, qt_meta_tag_ZN9LlmClientE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject LlmClient::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9LlmClientE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9LlmClientE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN9LlmClientE_t>.metaTypes,
    nullptr
} };

void LlmClient::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<LlmClient *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->completed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 1: _t->failed((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1]))); break;
        case 2: _t->apiBaseUrlChanged(); break;
        case 3: _t->apiKeyChanged(); break;
        case 4: _t->modelChanged(); break;
        case 5: _t->systemPromptChanged(); break;
        case 6: _t->temperatureChanged(); break;
        case 7: _t->maxTokensChanged(); break;
        case 8: _t->busyChanged(); break;
        case 9: _t->lastErrorChanged(); break;
        case 10: _t->requestEdit((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[4]))); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (LlmClient::*)(const QString & )>(_a, &LlmClient::completed, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (LlmClient::*)(const QString & )>(_a, &LlmClient::failed, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (LlmClient::*)()>(_a, &LlmClient::apiBaseUrlChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (LlmClient::*)()>(_a, &LlmClient::apiKeyChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (LlmClient::*)()>(_a, &LlmClient::modelChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (LlmClient::*)()>(_a, &LlmClient::systemPromptChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (LlmClient::*)()>(_a, &LlmClient::temperatureChanged, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (LlmClient::*)()>(_a, &LlmClient::maxTokensChanged, 7))
            return;
        if (QtMocHelpers::indexOfMethod<void (LlmClient::*)()>(_a, &LlmClient::busyChanged, 8))
            return;
        if (QtMocHelpers::indexOfMethod<void (LlmClient::*)()>(_a, &LlmClient::lastErrorChanged, 9))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QString*>(_v) = _t->apiBaseUrl(); break;
        case 1: *reinterpret_cast<QString*>(_v) = _t->apiKey(); break;
        case 2: *reinterpret_cast<QString*>(_v) = _t->model(); break;
        case 3: *reinterpret_cast<QString*>(_v) = _t->systemPrompt(); break;
        case 4: *reinterpret_cast<double*>(_v) = _t->temperature(); break;
        case 5: *reinterpret_cast<int*>(_v) = _t->maxTokens(); break;
        case 6: *reinterpret_cast<bool*>(_v) = _t->busy(); break;
        case 7: *reinterpret_cast<QString*>(_v) = _t->lastError(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setApiBaseUrl(*reinterpret_cast<QString*>(_v)); break;
        case 1: _t->setApiKey(*reinterpret_cast<QString*>(_v)); break;
        case 2: _t->setModel(*reinterpret_cast<QString*>(_v)); break;
        case 3: _t->setSystemPrompt(*reinterpret_cast<QString*>(_v)); break;
        case 4: _t->setTemperature(*reinterpret_cast<double*>(_v)); break;
        case 5: _t->setMaxTokens(*reinterpret_cast<int*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *LlmClient::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *LlmClient::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN9LlmClientE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int LlmClient::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 11)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 11;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}

// SIGNAL 0
void LlmClient::completed(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 0, nullptr, _t1);
}

// SIGNAL 1
void LlmClient::failed(const QString & _t1)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 1, nullptr, _t1);
}

// SIGNAL 2
void LlmClient::apiBaseUrlChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void LlmClient::apiKeyChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void LlmClient::modelChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void LlmClient::systemPromptChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void LlmClient::temperatureChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void LlmClient::maxTokensChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 7, nullptr);
}

// SIGNAL 8
void LlmClient::busyChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 8, nullptr);
}

// SIGNAL 9
void LlmClient::lastErrorChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 9, nullptr);
}
QT_WARNING_POP
