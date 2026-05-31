/****************************************************************************
** Meta object code from reading C++ file 'documentcontroller.h'
**
** Created by: The Qt Meta Object Compiler version 69 (Qt 6.11.1)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../../src/documentcontroller.h"
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'documentcontroller.h' doesn't include <QObject>."
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
struct qt_meta_tag_ZN18DocumentControllerE_t {};
} // unnamed namespace

template <> constexpr inline auto DocumentController::qt_create_metaobjectdata<qt_meta_tag_ZN18DocumentControllerE_t>()
{
    namespace QMC = QtMocConstants;
    QtMocHelpers::StringRefStorage qt_stringData {
        "DocumentController",
        "textChanged",
        "",
        "filePathChanged",
        "modifiedChanged",
        "windowTitleChanged",
        "recentFilesChanged",
        "prefersPackagingChanged",
        "unstagedAssetCountChanged",
        "saveDialogRequested",
        "saveAs",
        "suggestedPath",
        "defaultPackage",
        "unstagedCount",
        "showDiscard",
        "newDocument",
        "open",
        "openFile",
        "path",
        "openRecent",
        "save",
        "quit",
        "canClose",
        "markModified",
        "pickImageFile",
        "pickPdfExportFile",
        "pickSaveFolder",
        "documentDirectory",
        "markdownPath",
        "absolutePath",
        "suggestedSavePath",
        "clipboardHasImage",
        "pasteImageFromClipboard",
        "defaultPackageOnSave",
        "performSave",
        "packageAssets",
        "discardChanges",
        "text",
        "filePath",
        "modified",
        "windowTitle",
        "recentFiles",
        "prefersPackaging",
        "unstagedAssetCount"
    };

    QtMocHelpers::UintData qt_methods {
        // Signal 'textChanged'
        QtMocHelpers::SignalData<void()>(1, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'filePathChanged'
        QtMocHelpers::SignalData<void()>(3, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'modifiedChanged'
        QtMocHelpers::SignalData<void()>(4, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'windowTitleChanged'
        QtMocHelpers::SignalData<void()>(5, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'recentFilesChanged'
        QtMocHelpers::SignalData<void()>(6, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'prefersPackagingChanged'
        QtMocHelpers::SignalData<void()>(7, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'unstagedAssetCountChanged'
        QtMocHelpers::SignalData<void()>(8, 2, QMC::AccessPublic, QMetaType::Void),
        // Signal 'saveDialogRequested'
        QtMocHelpers::SignalData<void(bool, const QString &, bool, int, bool)>(9, 2, QMC::AccessPublic, QMetaType::Void, {{
            { QMetaType::Bool, 10 }, { QMetaType::QString, 11 }, { QMetaType::Bool, 12 }, { QMetaType::Int, 13 },
            { QMetaType::Bool, 14 },
        }}),
        // Method 'newDocument'
        QtMocHelpers::MethodData<bool()>(15, 2, QMC::AccessPublic, QMetaType::Bool),
        // Method 'open'
        QtMocHelpers::MethodData<bool()>(16, 2, QMC::AccessPublic, QMetaType::Bool),
        // Method 'openFile'
        QtMocHelpers::MethodData<bool(const QString &)>(17, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 18 },
        }}),
        // Method 'openRecent'
        QtMocHelpers::MethodData<bool(const QString &)>(19, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 18 },
        }}),
        // Method 'save'
        QtMocHelpers::MethodData<bool()>(20, 2, QMC::AccessPublic, QMetaType::Bool),
        // Method 'saveAs'
        QtMocHelpers::MethodData<bool()>(10, 2, QMC::AccessPublic, QMetaType::Bool),
        // Method 'quit'
        QtMocHelpers::MethodData<void()>(21, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'canClose'
        QtMocHelpers::MethodData<bool()>(22, 2, QMC::AccessPublic, QMetaType::Bool),
        // Method 'markModified'
        QtMocHelpers::MethodData<void()>(23, 2, QMC::AccessPublic, QMetaType::Void),
        // Method 'pickImageFile'
        QtMocHelpers::MethodData<QString()>(24, 2, QMC::AccessPublic, QMetaType::QString),
        // Method 'pickPdfExportFile'
        QtMocHelpers::MethodData<QString()>(25, 2, QMC::AccessPublic, QMetaType::QString),
        // Method 'pickSaveFolder'
        QtMocHelpers::MethodData<QString()>(26, 2, QMC::AccessPublic, QMetaType::QString),
        // Method 'documentDirectory'
        QtMocHelpers::MethodData<QString() const>(27, 2, QMC::AccessPublic, QMetaType::QString),
        // Method 'markdownPath'
        QtMocHelpers::MethodData<QString(const QString &) const>(28, 2, QMC::AccessPublic, QMetaType::QString, {{
            { QMetaType::QString, 29 },
        }}),
        // Method 'suggestedSavePath'
        QtMocHelpers::MethodData<QString() const>(30, 2, QMC::AccessPublic, QMetaType::QString),
        // Method 'clipboardHasImage'
        QtMocHelpers::MethodData<bool() const>(31, 2, QMC::AccessPublic, QMetaType::Bool),
        // Method 'pasteImageFromClipboard'
        QtMocHelpers::MethodData<QString()>(32, 2, QMC::AccessPublic, QMetaType::QString),
        // Method 'defaultPackageOnSave'
        QtMocHelpers::MethodData<bool() const>(33, 2, QMC::AccessPublic, QMetaType::Bool),
        // Method 'performSave'
        QtMocHelpers::MethodData<bool(const QString &, bool)>(34, 2, QMC::AccessPublic, QMetaType::Bool, {{
            { QMetaType::QString, 18 }, { QMetaType::Bool, 35 },
        }}),
        // Method 'discardChanges'
        QtMocHelpers::MethodData<void()>(36, 2, QMC::AccessPublic, QMetaType::Void),
    };
    QtMocHelpers::UintData qt_properties {
        // property 'text'
        QtMocHelpers::PropertyData<QString>(37, QMetaType::QString, QMC::DefaultPropertyFlags | QMC::Writable | QMC::StdCppSet, 0),
        // property 'filePath'
        QtMocHelpers::PropertyData<QString>(38, QMetaType::QString, QMC::DefaultPropertyFlags, 1),
        // property 'modified'
        QtMocHelpers::PropertyData<bool>(39, QMetaType::Bool, QMC::DefaultPropertyFlags, 2),
        // property 'windowTitle'
        QtMocHelpers::PropertyData<QString>(40, QMetaType::QString, QMC::DefaultPropertyFlags, 3),
        // property 'recentFiles'
        QtMocHelpers::PropertyData<QStringList>(41, QMetaType::QStringList, QMC::DefaultPropertyFlags, 4),
        // property 'prefersPackaging'
        QtMocHelpers::PropertyData<bool>(42, QMetaType::Bool, QMC::DefaultPropertyFlags, 5),
        // property 'unstagedAssetCount'
        QtMocHelpers::PropertyData<int>(43, QMetaType::Int, QMC::DefaultPropertyFlags, 6),
    };
    QtMocHelpers::UintData qt_enums {
    };
    return QtMocHelpers::metaObjectData<DocumentController, qt_meta_tag_ZN18DocumentControllerE_t>(QMC::MetaObjectFlag{}, qt_stringData,
            qt_methods, qt_properties, qt_enums);
}
Q_CONSTINIT const QMetaObject DocumentController::staticMetaObject = { {
    QMetaObject::SuperData::link<QObject::staticMetaObject>(),
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18DocumentControllerE_t>.stringdata,
    qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18DocumentControllerE_t>.data,
    qt_static_metacall,
    nullptr,
    qt_staticMetaObjectRelocatingContent<qt_meta_tag_ZN18DocumentControllerE_t>.metaTypes,
    nullptr
} };

void DocumentController::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<DocumentController *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->textChanged(); break;
        case 1: _t->filePathChanged(); break;
        case 2: _t->modifiedChanged(); break;
        case 3: _t->windowTitleChanged(); break;
        case 4: _t->recentFilesChanged(); break;
        case 5: _t->prefersPackagingChanged(); break;
        case 6: _t->unstagedAssetCountChanged(); break;
        case 7: _t->saveDialogRequested((*reinterpret_cast<std::add_pointer_t<bool>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<QString>>(_a[2])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[3])),(*reinterpret_cast<std::add_pointer_t<int>>(_a[4])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[5]))); break;
        case 8: { bool _r = _t->newDocument();
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 9: { bool _r = _t->open();
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 10: { bool _r = _t->openFile((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 11: { bool _r = _t->openRecent((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 12: { bool _r = _t->save();
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 13: { bool _r = _t->saveAs();
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 14: _t->quit(); break;
        case 15: { bool _r = _t->canClose();
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 16: _t->markModified(); break;
        case 17: { QString _r = _t->pickImageFile();
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 18: { QString _r = _t->pickPdfExportFile();
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 19: { QString _r = _t->pickSaveFolder();
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 20: { QString _r = _t->documentDirectory();
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 21: { QString _r = _t->markdownPath((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])));
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 22: { QString _r = _t->suggestedSavePath();
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 23: { bool _r = _t->clipboardHasImage();
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 24: { QString _r = _t->pasteImageFromClipboard();
            if (_a[0]) *reinterpret_cast<QString*>(_a[0]) = std::move(_r); }  break;
        case 25: { bool _r = _t->defaultPackageOnSave();
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 26: { bool _r = _t->performSave((*reinterpret_cast<std::add_pointer_t<QString>>(_a[1])),(*reinterpret_cast<std::add_pointer_t<bool>>(_a[2])));
            if (_a[0]) *reinterpret_cast<bool*>(_a[0]) = std::move(_r); }  break;
        case 27: _t->discardChanges(); break;
        default: ;
        }
    }
    if (_c == QMetaObject::IndexOfMethod) {
        if (QtMocHelpers::indexOfMethod<void (DocumentController::*)()>(_a, &DocumentController::textChanged, 0))
            return;
        if (QtMocHelpers::indexOfMethod<void (DocumentController::*)()>(_a, &DocumentController::filePathChanged, 1))
            return;
        if (QtMocHelpers::indexOfMethod<void (DocumentController::*)()>(_a, &DocumentController::modifiedChanged, 2))
            return;
        if (QtMocHelpers::indexOfMethod<void (DocumentController::*)()>(_a, &DocumentController::windowTitleChanged, 3))
            return;
        if (QtMocHelpers::indexOfMethod<void (DocumentController::*)()>(_a, &DocumentController::recentFilesChanged, 4))
            return;
        if (QtMocHelpers::indexOfMethod<void (DocumentController::*)()>(_a, &DocumentController::prefersPackagingChanged, 5))
            return;
        if (QtMocHelpers::indexOfMethod<void (DocumentController::*)()>(_a, &DocumentController::unstagedAssetCountChanged, 6))
            return;
        if (QtMocHelpers::indexOfMethod<void (DocumentController::*)(bool , const QString & , bool , int , bool )>(_a, &DocumentController::saveDialogRequested, 7))
            return;
    }
    if (_c == QMetaObject::ReadProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: *reinterpret_cast<QString*>(_v) = _t->text(); break;
        case 1: *reinterpret_cast<QString*>(_v) = _t->filePath(); break;
        case 2: *reinterpret_cast<bool*>(_v) = _t->modified(); break;
        case 3: *reinterpret_cast<QString*>(_v) = _t->windowTitle(); break;
        case 4: *reinterpret_cast<QStringList*>(_v) = _t->recentFiles(); break;
        case 5: *reinterpret_cast<bool*>(_v) = _t->prefersPackaging(); break;
        case 6: *reinterpret_cast<int*>(_v) = _t->unstagedAssetCount(); break;
        default: break;
        }
    }
    if (_c == QMetaObject::WriteProperty) {
        void *_v = _a[0];
        switch (_id) {
        case 0: _t->setText(*reinterpret_cast<QString*>(_v)); break;
        default: break;
        }
    }
}

const QMetaObject *DocumentController::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *DocumentController::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_staticMetaObjectStaticContent<qt_meta_tag_ZN18DocumentControllerE_t>.strings))
        return static_cast<void*>(this);
    return QObject::qt_metacast(_clname);
}

int DocumentController::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 28)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 28;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 28)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 28;
    }
    if (_c == QMetaObject::ReadProperty || _c == QMetaObject::WriteProperty
            || _c == QMetaObject::ResetProperty || _c == QMetaObject::BindableProperty
            || _c == QMetaObject::RegisterPropertyMetaType) {
        qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void DocumentController::textChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 0, nullptr);
}

// SIGNAL 1
void DocumentController::filePathChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 1, nullptr);
}

// SIGNAL 2
void DocumentController::modifiedChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 2, nullptr);
}

// SIGNAL 3
void DocumentController::windowTitleChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 3, nullptr);
}

// SIGNAL 4
void DocumentController::recentFilesChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 4, nullptr);
}

// SIGNAL 5
void DocumentController::prefersPackagingChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 5, nullptr);
}

// SIGNAL 6
void DocumentController::unstagedAssetCountChanged()
{
    QMetaObject::activate(this, &staticMetaObject, 6, nullptr);
}

// SIGNAL 7
void DocumentController::saveDialogRequested(bool _t1, const QString & _t2, bool _t3, int _t4, bool _t5)
{
    QMetaObject::activate<void>(this, &staticMetaObject, 7, nullptr, _t1, _t2, _t3, _t4, _t5);
}
QT_WARNING_POP
