import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import QtQuick.Dialogs
import QtCore
import org.kde.kirigami as Kirigami
import io.github.gottstaff.SimpleMD

Kirigami.ApplicationWindow {
    id: root

    title: document.windowTitle

    // Full-screen via showFullScreen(), not a visibility binding: Plasma and other
    // WMs overwrite visibility when focus changes, which breaks a one-way binding
    // and leaves a resized window with system title-bar decorations.
    property bool wantExpanded: true
    property bool windowWasMinimized: false
    property bool restoringWindow: false

    Timer {
        id: restoreFullscreenTimer
        interval: 120
        repeat: false
        onTriggered: root.restoreExpandedWindow()
    }

    Component.onCompleted: {
        editorHelper.installFileDrop(root)
        if (wantExpanded) {
            showFullScreen()
        }
        openExternalPaths(appIntegration.pendingFiles)
        credentialStore.loadApiKey()
    }

    function focusWindow() {
        raise()
        requestActivate()
    }

    function openExternalPaths(paths) {
        if (!paths || paths.length === 0) {
            return
        }
        focusWindow()
        document.openRecent(paths[0])
    }

    Connections {
        target: appIntegration

        function onOpenFilesRequested(paths) {
            focusWindow()
            if (paths.length > 0) {
                document.openRecent(paths[0])
            }
        }
    }

    function restoreExpandedWindow() {
        if (!wantExpanded
                || restoringWindow
                || visibility === Window.FullScreen
                || visibility === Window.Minimized
                || visibility === Window.Hidden) {
            return
        }
        restoringWindow = true
        Qt.callLater(function() {
            showFullScreen()
            restoringWindow = false
        })
    }

    onVisibilityChanged: function(visibility) {
        if (visibility === Window.Minimized) {
            restoreFullscreenTimer.stop()
            windowWasMinimized = true
            editorPane.onWindowMinimized()
            return
        }
        if (windowWasMinimized && visibility !== Window.Hidden) {
            windowWasMinimized = false
            editorPane.onWindowRestored()
        }
        if (wantExpanded && visibility !== Window.FullScreen) {
            // Full-screen exit precedes minimize on many WMs; defer restore so
            // showMinimized() can finish instead of snapping back to full screen.
            restoreFullscreenTimer.restart()
        }
    }

    onActiveChanged: {
        if (!active) {
            return
        }
        if (windowWasMinimized) {
            windowWasMinimized = false
            editorPane.onWindowRestored()
        }
        restoreExpandedWindow()
    }

    onClosing: close => {
        if (!document.canClose()) {
            close.accepted = false
        }
    }

    DocumentController {
        id: document
    }

    Connections {
        target: document

        function onSaveDialogRequested(saveAs, suggestedPath, defaultPackage, unstagedCount, showDiscard) {
            saveDialog.openWith(saveAs, suggestedPath, defaultPackage, unstagedCount, showDiscard)
        }
    }

    property alias editor: editorPane.editor

    Controls.Action {
        id: documentOutlineAction
        text: editorPane.outlineVisible
            ? i18nc("@action", "Hide Document Outline")
            : i18nc("@action", "Document Outline")
        checkable: true
        checked: editorPane.outlineVisible
        shortcut: "Ctrl+Shift+O"
        onTriggered: editorPane.toggleDocumentOutline()
    }

    QtObject {
        id: preferences

        property string editorFontFamily: ""
        property int editorFontSize: Kirigami.Theme.defaultFont.pixelSize

        property string previewFontFamily: Kirigami.Theme.defaultFont.family
        property int previewFontSize: Kirigami.Theme.defaultFont.pixelSize + 1
        property real previewLineHeight: 1.55

        property string aiApiBaseUrl: "https://api.openai.com/v1"
        property string aiModel: "gpt-4o-mini"
        property real aiTemperature: 0.4
        property int aiMaxTokens: 700
        readonly property string defaultAiSystemPrompt: "You are a precise assistant for editing markdown. Return only the final text to insert. For Mermaid diagrams use flowchart (not graph), and double-quote labels with parentheses or special characters, e.g. E[\"Energy density S(f)\"]."
        property string aiSystemPrompt: defaultAiSystemPrompt

        property string pdfLayoutPreset: "document"
        property string pdfTheme: "classic"
        property string pdfPageSize: "A4"
        property string pdfOrientation: "portrait"
        property string pdfMargins: "normal"
        property bool pdfPageNumbers: true

        property bool previewVisible: true
        property bool showWhitespace: false
        property bool syntaxHighlighting: false

        property string authorName: ""
    }

    Settings {
        category: "Preferences"
        property alias editorFontFamily: preferences.editorFontFamily
        property alias editorFontSize: preferences.editorFontSize
        property alias previewFontFamily: preferences.previewFontFamily
        property alias previewFontSize: preferences.previewFontSize
        property alias previewLineHeight: preferences.previewLineHeight
        property alias aiApiBaseUrl: preferences.aiApiBaseUrl
        property alias aiModel: preferences.aiModel
        property alias aiTemperature: preferences.aiTemperature
        property alias aiMaxTokens: preferences.aiMaxTokens
        property alias aiSystemPrompt: preferences.aiSystemPrompt
        property alias pdfLayoutPreset: preferences.pdfLayoutPreset
        property alias pdfTheme: preferences.pdfTheme
        property alias pdfPageSize: preferences.pdfPageSize
        property alias pdfOrientation: preferences.pdfOrientation
        property alias pdfMargins: preferences.pdfMargins
        property alias pdfPageNumbers: preferences.pdfPageNumbers
        property alias previewVisible: preferences.previewVisible
        property alias showWhitespace: preferences.showWhitespace
        property alias syntaxHighlighting: preferences.syntaxHighlighting
        property alias authorName: preferences.authorName
    }

    CredentialStore {
        id: credentialStore
    }

    LlmClient {
        id: llmClient
        apiBaseUrl: preferences.aiApiBaseUrl
        apiKey: credentialStore.apiKey
        model: preferences.aiModel
        temperature: preferences.aiTemperature
        maxTokens: preferences.aiMaxTokens
        systemPrompt: preferences.aiSystemPrompt
    }

    EditorHelper {
        id: editorHelper
    }

    PdfExportThemes {
        id: pdfExportThemes
    }

    Connections {
        target: editorHelper

        function onFilesDropped(paths, x, y) {
            editorPane.handleFileDrop(paths)
        }
    }

    menuBar: Rectangle {
        implicitHeight: menuBarRow.implicitHeight + Kirigami.Units.smallSpacing * 2
        color: Qt.tint(Kirigami.Theme.backgroundColor,
                       Qt.rgba(Kirigami.Theme.focusColor.r,
                               Kirigami.Theme.focusColor.g,
                               Kirigami.Theme.focusColor.b,
                               0.03))
        border.color: Qt.rgba(Kirigami.Theme.focusColor.r,
                              Kirigami.Theme.focusColor.g,
                              Kirigami.Theme.focusColor.b,
                              0.10)
        border.width: 0.5

        RowLayout {
            id: menuBarRow
            anchors.fill: parent
            anchors.leftMargin: Kirigami.Units.smallSpacing
            anchors.rightMargin: Kirigami.Units.smallSpacing
            spacing: Kirigami.Units.smallSpacing

            Controls.MenuBar {
                Layout.fillWidth: true
                background: null
                padding: 0
                spacing: Kirigami.Units.largeSpacing

        Controls.Menu {
            title: i18nc("@menu", "&File")

            Controls.MenuItem {
                action: Controls.Action {
                    text: i18nc("@action", "&New")
                    shortcut: StandardKey.New
                    onTriggered: document.newDocument()
                }
            }
            Controls.MenuItem {
                action: Controls.Action {
                    text: i18nc("@action", "&Open…")
                    shortcut: StandardKey.Open
                    onTriggered: document.open()
                }
            }
            Controls.Menu {
                id: recentFilesMenu

                title: i18nc("@menu", "Open &Recent")
                enabled: document.recentFiles.length > 0

                Instantiator {
                    model: document.recentFiles

                    delegate: Controls.MenuItem {
                        required property string modelData

                        text: {
                            const slash = Math.max(modelData.lastIndexOf("/"), modelData.lastIndexOf("\\"))
                            return slash >= 0 ? modelData.substring(slash + 1) : modelData
                        }

                        Controls.ToolTip.visible: hovered
                        Controls.ToolTip.text: modelData

                        onTriggered: document.openRecent(modelData)
                    }

                    onObjectAdded: (index, object) => recentFilesMenu.insertItem(index, object)
                    onObjectRemoved: (index, object) => recentFilesMenu.removeItem(object)
                }
            }
            Controls.MenuItem {
                action: Controls.Action {
                    text: i18nc("@action", "&Save")
                    shortcut: StandardKey.Save
                    enabled: document.modified
                    onTriggered: document.save()
                }
            }
            Controls.MenuItem {
                action: Controls.Action {
                    text: i18nc("@action", "Save &As…")
                    shortcut: StandardKey.SaveAs
                    onTriggered: document.saveAs()
                }
            }
            Controls.MenuSeparator {}
            Controls.MenuItem {
                action: Controls.Action {
                    text: i18nc("@action", "&Print…")
                    shortcut: StandardKey.Print
                    onTriggered: editorPane.printDocument()
                }
            }
            Controls.MenuItem {
                action: Controls.Action {
                    text: i18nc("@action", "Export to &PDF…")
                    shortcut: "Ctrl+Shift+E"
                    onTriggered: pdfExportDialog.open()
                }
            }
            Controls.MenuSeparator {}
            Controls.MenuItem {
                action: Controls.Action {
                    text: i18nc("@action", "E&xit")
                    shortcut: StandardKey.Quit
                    onTriggered: document.quit()
                }
            }
        }

        Controls.Menu {
            title: i18nc("@menu", "&Edit")

            Controls.MenuItem {
                action: Controls.Action {
                    text: i18nc("@action", "&Undo")
                    shortcut: StandardKey.Undo
                    enabled: editor.canUndo
                    onTriggered: editor.undo()
                }
            }
            Controls.MenuItem {
                action: Controls.Action {
                    text: i18nc("@action", "&Redo")
                    shortcut: StandardKey.Redo
                    enabled: editor.canRedo
                    onTriggered: editor.redo()
                }
            }
            Controls.MenuSeparator {}
            Controls.MenuItem {
                action: Controls.Action {
                    text: i18nc("@action", "Cu&t")
                    shortcut: StandardKey.Cut
                    enabled: editor.selectedText.length > 0
                    onTriggered: editor.cut()
                }
            }
            Controls.MenuItem {
                action: Controls.Action {
                    text: i18nc("@action", "&Copy")
                    shortcut: StandardKey.Copy
                    enabled: editor.selectedText.length > 0
                    onTriggered: editor.copy()
                }
            }
            Controls.MenuItem {
                action: Controls.Action {
                    text: i18nc("@action", "&Paste")
                    shortcut: StandardKey.Paste
                    enabled: editor.canPaste || document.clipboardHasImage()
                    onTriggered: editor.paste()
                }
            }
            Controls.MenuItem {
                action: Controls.Action {
                    text: i18nc("@action", "Select &All")
                    shortcut: StandardKey.SelectAll
                    onTriggered: editorPane.selectAll()
                }
            }
            Controls.MenuSeparator {}
            Controls.MenuItem {
                action: Controls.Action {
                    text: i18nc("@action", "&Find…")
                    shortcut: StandardKey.Find
                    onTriggered: findDialog.open()
                }
            }
            Controls.MenuItem {
                action: Controls.Action {
                    text: i18nc("@action", "Find and Re&place…")
                    shortcut: StandardKey.Replace
                    onTriggered: findReplaceDialog.open()
                }
            }
        }

        Controls.Menu {
            title: i18nc("@menu", "&View")

            Controls.MenuItem {
                action: Controls.Action {
                    text: preferences.previewVisible
                        ? i18nc("@action", "Hide Preview")
                        : i18nc("@action", "Show Preview")
                    shortcut: "F9"
                    onTriggered: editorPane.togglePreview()
                }
            }
            Controls.MenuItem {
                action: Controls.Action {
                    text: i18nc("@action", "Show &Whitespace")
                    checkable: true
                    checked: preferences.showWhitespace
                    onTriggered: preferences.showWhitespace = !preferences.showWhitespace
                }
            }
            Controls.MenuItem {
                action: Controls.Action {
                    text: i18nc("@action", "Syntax &Highlighting")
                    checkable: true
                    checked: preferences.syntaxHighlighting
                    onTriggered: preferences.syntaxHighlighting = !preferences.syntaxHighlighting
                }
            }
            Controls.MenuItem {
                action: documentOutlineAction
            }
        }

        Controls.Menu {
            title: i18nc("@menu", "&Insert")

            Controls.Menu {
                title: i18nc("@menu", "&Structure")

                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "Heading &1")
                        onTriggered: editorPane.insertHeading(1)
                    }
                }
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "Heading &2")
                        onTriggered: editorPane.insertHeading(2)
                    }
                }
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "Heading &3")
                        onTriggered: editorPane.insertHeading(3)
                    }
                }
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "Heading &4")
                        onTriggered: editorPane.insertHeading(4)
                    }
                }
                Controls.MenuSeparator {}
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "&Paragraph")
                        onTriggered: editorPane.insertBlock("Paragraph text.\n\n", 0, 15)
                    }
                }
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "Horizontal &Rule")
                        onTriggered: editorPane.insertBlock("---\n\n", 0, 0)
                    }
                }
            }

            Controls.Menu {
                title: i18nc("@menu", "&Lists")

                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "Bullet &List")
                        onTriggered: editorPane.insertBlock("- Item one\n- Item two\n- Item three\n\n", 2, 8)
                    }
                }
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "&Numbered List")
                        onTriggered: editorPane.insertBlock("1. First item\n2. Second item\n3. Third item\n\n", 3, 10)
                    }
                }
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "&Task List")
                        onTriggered: editorPane.insertBlock("- [ ] Todo item\n- [x] Done item\n\n", 4, 9)
                    }
                }
                Controls.MenuSeparator {}
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "&Block Quote")
                        onTriggered: editorPane.insertBlock("> Quoted text.\n\n", 2, 11)
                    }
                }
            }

            Controls.Menu {
                title: i18nc("@menu", "&Formatting")

                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "&Bold")
                        onTriggered: editorPane.wrapSelection("**", "**")
                    }
                }
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "&Italic")
                        onTriggered: editorPane.wrapSelection("*", "*")
                    }
                }
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "Inline &Code")
                        onTriggered: editorPane.wrapSelection("`", "`")
                    }
                }
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "&Link…")
                        onTriggered: linkDialog.open()
                    }
                }
                Controls.MenuSeparator {}
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "Code &Block")
                        onTriggered: editorPane.insertBlock("```\ncode\n```\n\n", 4, 4)
                    }
                }
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "&Table")
                        onTriggered: editorPane.insertBlock("| Column A | Column B |\n| --- | --- |\n| Cell 1 | Cell 2 |\n\n", 2, 8)
                    }
                }
            }

            Controls.Menu {
                title: i18nc("@menu", "&Math")

                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "Inline &Math")
                        onTriggered: editorPane.insertInline("$", "$", "E = mc^2")
                    }
                }
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "Display &Math")
                        onTriggered: editorPane.insertBlock("$$\n\\int_0^1 x\\,dx\n$$\n\n", 3, 16)
                    }
                }
            }

            Controls.Menu {
                title: i18nc("@menu", "C&hemistry")

                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "Inline &Formula")
                        onTriggered: editorPane.insertChemicalFormula("H2O")
                    }
                }
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "Chemical &Equation")
                        onTriggered: editorPane.insertChemicalEquation("2H2 + O2 -> 2H2O")
                    }
                }
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "Physical &Unit")
                        onTriggered: editorPane.insertPhysicalUnit("mol/L")
                    }
                }
                Controls.MenuSeparator {}
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "Ionic &Compound")
                        onTriggered: editorPane.insertChemicalFormula("Na+ + Cl- -> NaCl")
                    }
                }
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "&Reaction with Conditions")
                        onTriggered: editorPane.insertChemicalEquation("CH4 + 2O2 ->[\Delta] CO2 + 2H2O")
                    }
                }
            }

            Controls.Menu {
                title: i18nc("@menu", "Media && &Figures")

                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "&Image…")
                        onTriggered: {
                            const path = document.pickImageFile()
                            if (path.length > 0) {
                                editorPane.insertImage(path)
                            }
                        }
                    }
                }
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "&Figure…")
                        onTriggered: figureDialog.open()
                    }
                }
            }

            Controls.Menu {
                title: i18nc("@menu", "&Diagrams")

                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "&Flowchart")
                        onTriggered: editorPane.insertMermaid("flowchart LR\n  A[Start] --> B{Decision}\n  B -->|Yes| C[End]\n  B -->|No| D[Retry]")
                    }
                }
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "&Sequence Diagram")
                        onTriggered: editorPane.insertMermaid("sequenceDiagram\n  Alice->>Bob: Hello\n  Bob-->>Alice: Hi")
                    }
                }
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "&Class Diagram")
                        onTriggered: editorPane.insertMermaid("classDiagram\n  Animal <|-- Duck\n  Animal : +int age\n  Duck : +swim()")
                    }
                }
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "&State Diagram")
                        onTriggered: editorPane.insertMermaid("stateDiagram-v2\n  [*] --> Idle\n  Idle --> Active\n  Active --> [*]")
                    }
                }
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "&Gantt Chart")
                        onTriggered: editorPane.insertMermaid("gantt\n  dateFormat YYYY-MM-DD\n  title Project\n  section Phase 1\n  Task A :a1, 2024-01-01, 7d\n  Task B :b1, after a1, 5d")
                    }
                }
            }

            Controls.Menu {
                title: i18nc("@menu", "&Snippets")

                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "&Author")
                        onTriggered: editorPane.insertAuthor(preferences.authorName)
                    }
                }
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "&Date")
                        onTriggered: editorPane.insertDate()
                    }
                }
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "Author and &Date")
                        onTriggered: editorPane.insertAuthorAndDate(preferences.authorName)
                    }
                }
                Controls.MenuSeparator {}
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "&Title")
                        onTriggered: editorPane.insertBlock("**Title:** Document title\n\n", 11, 14)
                    }
                }
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "&Abstract")
                        onTriggered: editorPane.insertBlock("## Abstract\n\nSummary text.\n\n", 12, 12)
                    }
                }
            }

            Controls.Menu {
                title: i18nc("@menu", "&References")

                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "Footnote &Reference")
                        onTriggered: editorPane.insertFootnoteReference("note")
                    }
                }
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "Footnote &Definition")
                        onTriggered: editorPane.insertFootnoteDefinition("note", "Author, Title, Publisher, year.")
                    }
                }
                Controls.MenuSeparator {}
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "Bibliography &Entry")
                        onTriggered: editorPane.insertBibliographyEntry("Author2024", "Author, *Title*, Publisher, 2024.")
                    }
                }
                Controls.MenuItem {
                    action: Controls.Action {
                        text: i18nc("@action", "Cite &Reference")
                        onTriggered: editorPane.insertInline("[^", "]", "Author2024")
                    }
                }
            }
        }

        Controls.Menu {
            title: i18nc("@menu", "&Settings")

            Controls.MenuItem {
                action: Controls.Action {
                    text: i18nc("@action", "&Preferences…")
                    shortcut: StandardKey.Preferences
                    onTriggered: preferencesDialog.open()
                }
            }
        }

        Controls.Menu {
            title: i18nc("@menu", "&AI")

            Controls.MenuItem {
                action: Controls.Action {
                    text: i18nc("@action", "Edit with &AI…")
                    shortcut: "Ctrl+Shift+I"
                    onTriggered: aiPromptDialog.open()
                }
            }
        }
            }

            RowLayout {
                spacing: 0

                Controls.ToolButton {
                    Layout.preferredWidth: Kirigami.Units.gridUnit * 2.4
                    Layout.preferredHeight: Kirigami.Units.gridUnit * 2.4
                    icon.name: "window-minimize-symbolic"
                    display: Controls.AbstractButton.IconOnly
                    flat: true
                    opacity: hovered ? 1.0 : 0.72
                    Controls.ToolTip.visible: hovered
                    Controls.ToolTip.text: i18nc("@action", "Minimize")
                    onClicked: root.showMinimized()
                }

                Controls.ToolButton {
                    Layout.preferredWidth: Kirigami.Units.gridUnit * 2.4
                    Layout.preferredHeight: Kirigami.Units.gridUnit * 2.4
                    icon.name: "window-close-symbolic"
                    display: Controls.AbstractButton.IconOnly
                    flat: true
                    opacity: hovered ? 1.0 : 0.72
                    Controls.ToolTip.visible: hovered
                    Controls.ToolTip.text: i18nc("@action", "Close")
                    onClicked: root.close()
                }
            }
        }
    }

    pageStack.initialPage: Kirigami.Page {
        title: ""
        globalToolBarStyle: Kirigami.ApplicationHeaderStyle.None
        leftPadding: 0
        rightPadding: 0
        topPadding: 0
        bottomPadding: 0

        SplitEditor {
            id: editorPane
            anchors.fill: parent
            document: document
            preferences: preferences
            editorHelper: editorHelper
        }
    }

    Controls.Dialog {
        id: saveDialog

        title: saveDialog.saveAsMode
               ? i18nc("@title:dialog", "Save As")
               : i18nc("@title:dialog", "Save")
        anchors.centerIn: parent
        modal: true
        focus: true
        width: Kirigami.Units.gridUnit * 30
        padding: Kirigami.Units.largeSpacing
        standardButtons: Controls.Dialog.Cancel

        property bool saveAsMode: false
        property bool showDiscardButton: false
        property bool pendingWindowClose: false
        property bool packageAssets: true

        function joinPath(folder, name) {
            const trimmedFolder = folder.replace(/[\\/]+$/, "")
            const trimmedName = name.replace(/^[\\/]+/, "")
            if (trimmedFolder.length === 0) {
                return trimmedName
            }
            if (trimmedName.length === 0) {
                return trimmedFolder
            }
            return trimmedFolder + "/" + trimmedName
        }

        function openWith(saveAs, suggestedPath, defaultPackage, unstagedCount, showDiscard) {
            saveDialog.saveAsMode = saveAs
            saveDialog.showDiscardButton = showDiscard
            saveDialog.pendingWindowClose = showDiscard
            saveDialog.packageAssets = defaultPackage
            packageCheckbox.checked = defaultPackage
            unstagedWarning.visible = unstagedCount > 0
            unstagedWarning.text = i18nc("@info", "%1 pasted image(s) are not saved in the project folder. Enable packaging to include them.", unstagedCount)

            const slash = Math.max(suggestedPath.lastIndexOf("/"), suggestedPath.lastIndexOf("\\"))
            if (slash >= 0) {
                folderField.text = suggestedPath.substring(0, slash)
                filenameField.text = suggestedPath.substring(slash + 1)
            } else {
                folderField.text = ""
                filenameField.text = suggestedPath
            }
            saveDialog.open()
        }

        footer: Controls.DialogButtonBox {
            alignment: Qt.AlignRight

            Controls.Button {
                visible: saveDialog.showDiscardButton
                text: i18nc("@action", "Discard")
                onClicked: {
                    document.discardChanges()
                    saveDialog.close()
                    if (saveDialog.pendingWindowClose) {
                        root.close()
                    }
                }
            }
            Controls.Button {
                text: i18nc("@action", "Save")
                enabled: saveDialog.joinPath(folderField.text, filenameField.text).length > 0
                onClicked: {
                    const path = saveDialog.joinPath(folderField.text, filenameField.text)
                    if (path.length === 0) {
                        return
                    }
                    if (document.performSave(path, packageCheckbox.checked)) {
                        saveDialog.close()
                        if (saveDialog.pendingWindowClose) {
                            root.close()
                        }
                    }
                }
            }
        }

        contentItem: ColumnLayout {
            spacing: Kirigami.Units.smallSpacing

            Controls.Label {
                text: i18nc("@label", "Folder")
            }
            RowLayout {
                Layout.fillWidth: true
                spacing: Kirigami.Units.smallSpacing

                Controls.TextField {
                    id: folderField
                    Layout.fillWidth: true
                    placeholderText: i18nc("@info:placeholder", "Choose a folder…")
                }
                Controls.Button {
                    text: i18nc("@action", "Browse…")
                    onClicked: {
                        const folder = document.pickSaveFolder()
                        if (folder.length > 0) {
                            folderField.text = folder
                        }
                    }
                }
            }

            Controls.Label {
                text: i18nc("@label", "File name")
            }
            Controls.TextField {
                id: filenameField
                Layout.fillWidth: true
                placeholderText: i18nc("@info:placeholder", "document.md")
            }

            Controls.CheckBox {
                id: packageCheckbox
                Layout.fillWidth: true
                text: i18nc("@option:checkbox", "Package images into project folder")
                checked: saveDialog.packageAssets
                onCheckedChanged: saveDialog.packageAssets = checked
            }

            Controls.Label {
                id: unstagedWarning
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                visible: false
                color: Kirigami.Theme.neutralTextColor
            }
        }

        onOpened: filenameField.forceActiveFocus()
    }

    Controls.Dialog {
        id: pdfExportDialog

        title: i18nc("@title:dialog", "Export to PDF")
        anchors.centerIn: parent
        modal: true
        focus: true
        width: Kirigami.Units.gridUnit * 26
        padding: Kirigami.Units.largeSpacing
        standardButtons: Controls.Dialog.Cancel
        background: Rectangle {
            color: Qt.tint(Kirigami.Theme.backgroundColor,
                           Qt.rgba(Kirigami.Theme.focusColor.r,
                                   Kirigami.Theme.focusColor.g,
                                   Kirigami.Theme.focusColor.b,
                                   0.02))
            border.color: Qt.rgba(Kirigami.Theme.textColor.r,
                                  Kirigami.Theme.textColor.g,
                                  Kirigami.Theme.textColor.b,
                                  0.12)
            border.width: 1
            radius: Kirigami.Units.cornerRadius
        }

        footer: Controls.DialogButtonBox {
            alignment: Qt.AlignRight
            Controls.Button {
                text: i18nc("@action", "Export")
                onClicked: {
                    const path = document.pickPdfExportFile()
                    if (path.length === 0) {
                        return
                    }
                    preferences.pdfLayoutPreset = exportLayoutCombo.currentValue
                    preferences.pdfTheme = exportThemeCombo.currentValue
                    preferences.pdfPageSize = exportPageSizeCombo.currentValue
                    preferences.pdfOrientation = exportOrientationCombo.currentValue
                    preferences.pdfMargins = exportMarginsCombo.currentValue
                    preferences.pdfPageNumbers = exportPageNumbersSwitch.checked
                    editorPane.exportDocument(path, {
                        layout: exportLayoutCombo.currentValue,
                        theme: exportThemeCombo.currentValue,
                        themeData: pdfExportThemes.themeData(exportThemeCombo.currentValue),
                        pageSize: exportPageSizeCombo.currentValue,
                        orientation: exportOrientationCombo.currentValue,
                        margins: exportMarginsCombo.currentValue,
                        pageNumbers: exportPageNumbersSwitch.checked
                    })
                    pdfExportDialog.close()
                }
            }
        }

        contentItem: ColumnLayout {
            spacing: Kirigami.Units.mediumSpacing

            Controls.Label {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                opacity: 0.7
                text: i18nc("@info", "Export the rendered preview as a PDF file. Layout settings are remembered for next time.")
            }

            GridLayout {
                Layout.fillWidth: true
                columns: 2
                rowSpacing: Kirigami.Units.smallSpacing
                columnSpacing: Kirigami.Units.largeSpacing

                Controls.Label { text: i18nc("@label", "Layout") }
                Controls.ComboBox {
                    id: exportLayoutCombo
                    Layout.fillWidth: true
                    model: [
                        { text: i18nc("@label", "Document"), value: "document" },
                        { text: i18nc("@label", "Compact"), value: "compact" },
                        { text: i18nc("@label", "Manuscript"), value: "manuscript" }
                    ]
                    textRole: "text"
                    valueRole: "value"
                    onActivated: pdfExportDialog.applyLayoutPreset(currentValue)
                }

                Controls.Label { text: i18nc("@label", "Theme") }
                Controls.ComboBox {
                    id: exportThemeCombo
                    Layout.fillWidth: true
                    model: pdfExportThemes.themes
                    textRole: "text"
                    valueRole: "value"
                }

                Controls.Label { text: i18nc("@label", "Page size") }
                Controls.ComboBox {
                    id: exportPageSizeCombo
                    Layout.fillWidth: true
                    model: [
                        { text: "A4", value: "A4" },
                        { text: i18nc("@label", "US Letter"), value: "Letter" },
                        { text: i18nc("@label", "US Legal"), value: "Legal" }
                    ]
                    textRole: "text"
                    valueRole: "value"
                }

                Controls.Label { text: i18nc("@label", "Orientation") }
                Controls.ComboBox {
                    id: exportOrientationCombo
                    Layout.fillWidth: true
                    model: [
                        { text: i18nc("@label", "Portrait"), value: "portrait" },
                        { text: i18nc("@label", "Landscape"), value: "landscape" }
                    ]
                    textRole: "text"
                    valueRole: "value"
                }

                Controls.Label { text: i18nc("@label", "Margins") }
                Controls.ComboBox {
                    id: exportMarginsCombo
                    Layout.fillWidth: true
                    model: [
                        { text: i18nc("@label", "Normal"), value: "normal" },
                        { text: i18nc("@label", "Narrow"), value: "narrow" },
                        { text: i18nc("@label", "Wide"), value: "wide" }
                    ]
                    textRole: "text"
                    valueRole: "value"
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: Kirigami.Units.smallSpacing

                Controls.Label {
                    text: i18nc("@label", "Page numbers")
                    Layout.fillWidth: true
                }
                Controls.Switch {
                    id: exportPageNumbersSwitch
                }
            }
        }

        function themeIndexForId(themeId) {
            const idx = exportThemeCombo.indexOfValue(themeId)
            if (idx >= 0) {
                return idx
            }
            const defaultIdx = exportThemeCombo.indexOfValue(pdfExportThemes.defaultThemeId)
            return defaultIdx >= 0 ? defaultIdx : 0
        }

        function syncFromPreferences() {
            exportLayoutCombo.currentIndex = Math.max(0, exportLayoutCombo.indexOfValue(preferences.pdfLayoutPreset))
            exportThemeCombo.currentIndex = themeIndexForId(preferences.pdfTheme)
            exportPageSizeCombo.currentIndex = Math.max(0, exportPageSizeCombo.indexOfValue(preferences.pdfPageSize))
            exportOrientationCombo.currentIndex = Math.max(0, exportOrientationCombo.indexOfValue(preferences.pdfOrientation))
            exportMarginsCombo.currentIndex = Math.max(0, exportMarginsCombo.indexOfValue(preferences.pdfMargins))
            exportPageNumbersSwitch.checked = preferences.pdfPageNumbers
        }

        function applyLayoutPreset(preset) {
            if (preset === "compact") {
                exportMarginsCombo.currentIndex = Math.max(0, exportMarginsCombo.indexOfValue("narrow"))
                exportPageNumbersSwitch.checked = false
            } else if (preset === "manuscript") {
                exportPageSizeCombo.currentIndex = Math.max(0, exportPageSizeCombo.indexOfValue("Letter"))
                exportMarginsCombo.currentIndex = Math.max(0, exportMarginsCombo.indexOfValue("wide"))
                exportOrientationCombo.currentIndex = Math.max(0, exportOrientationCombo.indexOfValue("portrait"))
                exportPageNumbersSwitch.checked = false
            } else {
                exportPageSizeCombo.currentIndex = Math.max(0, exportPageSizeCombo.indexOfValue("A4"))
                exportMarginsCombo.currentIndex = Math.max(0, exportMarginsCombo.indexOfValue("normal"))
                exportOrientationCombo.currentIndex = Math.max(0, exportOrientationCombo.indexOfValue("portrait"))
                exportPageNumbersSwitch.checked = true
            }
        }

        onOpened: {
            pdfExportThemes.reload()
            syncFromPreferences()
        }
    }

    Controls.Dialog {
        id: preferencesDialog

        title: i18nc("@title:dialog", "Preferences")
        anchors.centerIn: parent
        modal: true
        focus: true
        standardButtons: Controls.Dialog.Close
        width: Kirigami.Units.gridUnit * 26
        padding: Kirigami.Units.largeSpacing
        background: Rectangle {
            color: Qt.tint(Kirigami.Theme.backgroundColor,
                           Qt.rgba(Kirigami.Theme.focusColor.r,
                                   Kirigami.Theme.focusColor.g,
                                   Kirigami.Theme.focusColor.b,
                                   0.02))
            border.color: Qt.rgba(Kirigami.Theme.textColor.r,
                                  Kirigami.Theme.textColor.g,
                                  Kirigami.Theme.textColor.b,
                                  0.12)
            border.width: 1
            radius: Kirigami.Units.cornerRadius
        }

        contentItem: ColumnLayout {
            spacing: Kirigami.Units.mediumSpacing

            Controls.Label {
                text: i18nc("@label", "Typography")
                font.capitalization: Font.AllUppercase
                font.letterSpacing: 0.8
                opacity: 0.55
            }

            GridLayout {
                Layout.fillWidth: true
                columns: 2
                rowSpacing: Kirigami.Units.smallSpacing
                columnSpacing: Kirigami.Units.largeSpacing

                Controls.Label { text: i18nc("@label", "Editor font") }
                Controls.Button {
                    Layout.fillWidth: true
                    text: preferences.editorFontFamily + " • " + preferences.editorFontSize
                    onClicked: editorFontDialog.open()
                }

                Controls.Label { text: i18nc("@label", "Preview font") }
                Controls.Button {
                    Layout.fillWidth: true
                    text: preferences.previewFontFamily + " • " + preferences.previewFontSize
                    onClicked: previewFontDialog.open()
                }
            }

            Rectangle {
                Layout.fillWidth: true
                implicitHeight: 1
                color: Qt.rgba(Kirigami.Theme.textColor.r,
                               Kirigami.Theme.textColor.g,
                               Kirigami.Theme.textColor.b,
                               0.12)
            }

            Controls.Label {
                text: i18nc("@label", "Document")
                font.capitalization: Font.AllUppercase
                font.letterSpacing: 0.8
                opacity: 0.55
            }

            GridLayout {
                Layout.fillWidth: true
                columns: 2
                rowSpacing: Kirigami.Units.smallSpacing
                columnSpacing: Kirigami.Units.largeSpacing

                Controls.Label { text: i18nc("@label", "Author name") }
                Controls.TextField {
                    Layout.fillWidth: true
                    text: preferences.authorName
                    placeholderText: i18nc("@info:placeholder", "Your name")
                    onEditingFinished: preferences.authorName = text.trim()
                }
            }

            Rectangle {
                Layout.fillWidth: true
                implicitHeight: 1
                color: Qt.rgba(Kirigami.Theme.textColor.r,
                               Kirigami.Theme.textColor.g,
                               Kirigami.Theme.textColor.b,
                               0.12)
            }

            Controls.Label {
                text: i18nc("@label", "Reading")
                font.capitalization: Font.AllUppercase
                font.letterSpacing: 0.8
                opacity: 0.55
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: Kirigami.Units.smallSpacing

                Controls.Label {
                    text: i18nc("@label", "Line spacing")
                    Layout.preferredWidth: Kirigami.Units.gridUnit * 6
                }
                Controls.Slider {
                    from: 1.2
                    to: 1.9
                    stepSize: 0.05
                    value: preferences.previewLineHeight
                    onMoved: preferences.previewLineHeight = value
                    Layout.fillWidth: true
                }
                Controls.Label {
                    text: preferences.previewLineHeight.toFixed(2)
                    horizontalAlignment: Text.AlignRight
                    Layout.preferredWidth: Kirigami.Units.gridUnit * 2
                }
            }

            Rectangle {
                Layout.fillWidth: true
                implicitHeight: 1
                color: Qt.rgba(Kirigami.Theme.textColor.r,
                               Kirigami.Theme.textColor.g,
                               Kirigami.Theme.textColor.b,
                               0.12)
            }

            Controls.Label {
                text: i18nc("@label", "AI")
                font.capitalization: Font.AllUppercase
                font.letterSpacing: 0.8
                opacity: 0.55
            }

            GridLayout {
                Layout.fillWidth: true
                columns: 2
                rowSpacing: Kirigami.Units.smallSpacing
                columnSpacing: Kirigami.Units.largeSpacing

                Controls.Label { text: i18nc("@label", "API base URL") }
                Controls.TextField {
                    Layout.fillWidth: true
                    text: preferences.aiApiBaseUrl
                    placeholderText: "https://api.openai.com/v1"
                    onEditingFinished: preferences.aiApiBaseUrl = text.trim()
                }

                Controls.Label { text: i18nc("@label", "API key") }
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: Kirigami.Units.smallSpacing

                    Controls.TextField {
                        id: apiKeyField
                        Layout.fillWidth: true
                        text: credentialStore.apiKey
                        echoMode: TextInput.Password
                        placeholderText: "sk-..."
                        onEditingFinished: credentialStore.saveApiKey(text.trim())
                    }

                    Controls.Label {
                        Layout.fillWidth: true
                        wrapMode: Text.WordWrap
                        opacity: 0.65
                        font.pointSize: Kirigami.Theme.smallFont.pointSize
                        visible: credentialStore.loaded && credentialStore.storageMode.length > 0
                        text: {
                            if (credentialStore.storageMode === "keyring") {
                                return i18nc("@info", "Stored in system keyring")
                            }
                            if (credentialStore.storageMode === "session") {
                                return i18nc("@info", "Keyring unavailable — key kept for this session only")
                            }
                            return ""
                        }
                    }
                }

                Controls.Label { text: i18nc("@label", "Model") }
                Controls.TextField {
                    Layout.fillWidth: true
                    text: preferences.aiModel
                    placeholderText: "gpt-4o-mini / deepseek-chat"
                    onEditingFinished: preferences.aiModel = text.trim()
                }

                Controls.Label { text: i18nc("@label", "Temperature") }
                RowLayout {
                    Layout.fillWidth: true
                    Controls.Slider {
                        from: 0.0
                        to: 1.5
                        stepSize: 0.05
                        value: preferences.aiTemperature
                        onMoved: preferences.aiTemperature = value
                        Layout.fillWidth: true
                    }
                    Controls.Label {
                        text: preferences.aiTemperature.toFixed(2)
                        Layout.preferredWidth: Kirigami.Units.gridUnit * 2
                        horizontalAlignment: Text.AlignRight
                    }
                }

                Controls.Label { text: i18nc("@label", "Max tokens") }
                Controls.SpinBox {
                    Layout.fillWidth: true
                    from: 128
                    to: 4000
                    value: preferences.aiMaxTokens
                    editable: true
                    onValueModified: preferences.aiMaxTokens = value
                }
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: Kirigami.Units.smallSpacing

                Controls.Label {
                    text: i18nc("@label", "System prompt")
                    Layout.fillWidth: true
                }

                Controls.ToolButton {
                    icon.name: "edit-reset-symbolic"
                    display: Controls.AbstractButton.IconOnly
                    flat: true
                    opacity: hovered ? 1.0 : 0.65
                    enabled: preferences.aiSystemPrompt !== preferences.defaultAiSystemPrompt
                    Layout.preferredWidth: Kirigami.Units.gridUnit * 2
                    Layout.preferredHeight: Kirigami.Units.gridUnit * 2
                    Controls.ToolTip.visible: hovered
                    Controls.ToolTip.text: i18nc("@action", "Reset to default")
                    onClicked: preferences.aiSystemPrompt = preferences.defaultAiSystemPrompt
                }
            }

            Controls.ScrollView {
                id: systemPromptScroll

                Layout.fillWidth: true
                Layout.preferredHeight: Kirigami.Units.gridUnit * 4
                clip: true

                Controls.ScrollBar.vertical: StyledScrollBar {}

                Controls.TextArea {
                    id: systemPromptField

                    width: systemPromptScroll.availableWidth
                    implicitHeight: contentHeight + topPadding + bottomPadding
                    wrapMode: TextEdit.Wrap
                    text: preferences.aiSystemPrompt
                    onTextChanged: preferences.aiSystemPrompt = text
                }
            }
        }
    }

    FontDialog {
        id: editorFontDialog
        currentFont.family: preferences.editorFontFamily
        currentFont.pointSize: preferences.editorFontSize
        onAccepted: {
            preferences.editorFontFamily = selectedFont.family
            preferences.editorFontSize = selectedFont.pointSize
        }
    }

    FontDialog {
        id: previewFontDialog
        currentFont.family: preferences.previewFontFamily
        currentFont.pointSize: preferences.previewFontSize
        onAccepted: {
            preferences.previewFontFamily = selectedFont.family
            preferences.previewFontSize = selectedFont.pointSize
        }
    }

    Controls.Dialog {
        id: aiPromptDialog

        title: i18nc("@title:dialog", "Edit with AI")
        anchors.centerIn: parent
        modal: true
        focus: true
        width: Kirigami.Units.gridUnit * 28
        standardButtons: Controls.Dialog.Cancel
        padding: Kirigami.Units.largeSpacing
        background: Rectangle {
            color: Qt.tint(Kirigami.Theme.backgroundColor,
                           Qt.rgba(Kirigami.Theme.focusColor.r,
                                   Kirigami.Theme.focusColor.g,
                                   Kirigami.Theme.focusColor.b,
                                   0.02))
            border.color: Qt.rgba(Kirigami.Theme.textColor.r,
                                  Kirigami.Theme.textColor.g,
                                  Kirigami.Theme.textColor.b,
                                  0.12)
            border.width: 1
            radius: Kirigami.Units.cornerRadius
        }

        footer: Controls.DialogButtonBox {
            alignment: Qt.AlignRight

            Controls.Button {
                text: llmClient.busy ? i18nc("@action", "Working…") : i18nc("@action", "Apply")
                enabled: !llmClient.busy && aiRequestField.text.trim().length > 0
                onClicked: {
                    llmClient.requestEdit(
                        aiRequestField.text,
                        editorPane.selectedTextForAi(),
                        editorPane.beforeCursorContext(1200),
                        editorPane.afterCursorContext(1200)
                    )
                }
            }
        }

        contentItem: ColumnLayout {
            spacing: Kirigami.Units.smallSpacing

            Controls.Label {
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
                opacity: 0.7
                text: editor.selectedText.length > 0
                    ? i18nc("@info", "Selected text will be replaced by the model output.")
                    : i18nc("@info", "Output will be inserted at the current cursor position.")
            }

            Controls.TextArea {
                id: aiRequestField

                Layout.fillWidth: true
                Layout.preferredHeight: Kirigami.Units.gridUnit * 6
                wrapMode: TextEdit.Wrap
                placeholderText: i18nc("@info:placeholder", "Describe what you want to change…")
            }

            Controls.Label {
                visible: llmClient.lastError.length > 0
                color: Kirigami.Theme.negativeTextColor
                wrapMode: Text.WordWrap
                text: llmClient.lastError
            }
        }

        onOpened: {
            aiRequestField.forceActiveFocus()
            aiRequestField.selectAll()
        }
    }

    Connections {
        target: llmClient

        function onCompleted(text) {
            editorPane.applyAiResult(text)
            aiPromptDialog.close()
        }
    }

    Controls.Dialog {
        id: findDialog

        title: i18nc("@title:dialog", "Find")
        anchors.centerIn: parent
        modal: true
        focus: true
        standardButtons: Controls.Dialog.Close

        contentItem: ColumnLayout {
            spacing: Kirigami.Units.smallSpacing

            Controls.TextField {
                id: findField

                Layout.fillWidth: true
                placeholderText: i18nc("@info:placeholder", "Search text…")
                focus: true

                Keys.onReturnPressed: findNextAction.clicked()
                Keys.onEnterPressed: findNextAction.clicked()
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: Kirigami.Units.smallSpacing

                Controls.Button {
                    id: findNextAction
                    text: i18nc("@action", "Find Next")
                    onClicked: {
                        if (!editorPane.findNext(findField.text, false)) {
                            findField.selectAll()
                        }
                    }
                }
                Controls.Button {
                    text: i18nc("@action", "Find Previous")
                    onClicked: {
                        if (!editorPane.findNext(findField.text, true)) {
                            findField.selectAll()
                        }
                    }
                }
            }
        }

        onOpened: {
            findField.text = editor.selectedText
            findField.selectAll()
            findField.forceActiveFocus()
        }
    }

    Controls.Dialog {
        id: findReplaceDialog

        title: i18nc("@title:dialog", "Find and Replace")
        anchors.centerIn: parent
        modal: true
        focus: true
        standardButtons: Controls.Dialog.Close

        contentItem: ColumnLayout {
            spacing: Kirigami.Units.smallSpacing

            Controls.TextField {
                id: findReplaceFindField

                Layout.fillWidth: true
                placeholderText: i18nc("@info:placeholder", "Find…")
                focus: true

                Keys.onReturnPressed: findReplaceNextAction.clicked()
                Keys.onEnterPressed: findReplaceNextAction.clicked()
            }

            Controls.TextField {
                id: findReplaceReplaceField

                Layout.fillWidth: true
                placeholderText: i18nc("@info:placeholder", "Replace with…")

                Keys.onReturnPressed: findReplaceReplaceAction.clicked()
                Keys.onEnterPressed: findReplaceReplaceAction.clicked()
            }

            RowLayout {
                Layout.fillWidth: true
                spacing: Kirigami.Units.smallSpacing

                Controls.Button {
                    id: findReplaceNextAction
                    text: i18nc("@action", "Find Next")
                    onClicked: {
                        if (!editorPane.findNext(findReplaceFindField.text, false)) {
                            findReplaceFindField.selectAll()
                        }
                    }
                }
                Controls.Button {
                    text: i18nc("@action", "Find Previous")
                    onClicked: {
                        if (!editorPane.findNext(findReplaceFindField.text, true)) {
                            findReplaceFindField.selectAll()
                        }
                    }
                }
                Controls.Button {
                    text: i18nc("@action", "Replace")
                    id: findReplaceReplaceAction
                    onClicked: {
                        if (!editorPane.replaceNext(findReplaceFindField.text,
                                                     findReplaceReplaceField.text,
                                                     false)) {
                            findReplaceFindField.selectAll()
                        }
                    }
                }
                Controls.Button {
                    text: i18nc("@action", "Replace All")
                    onClicked: {
                        const count = editorPane.replaceAll(findReplaceFindField.text,
                                                            findReplaceReplaceField.text)
                        if (count === 0) {
                            findReplaceFindField.selectAll()
                        }
                    }
                }
            }
        }

        onOpened: {
            findReplaceFindField.text = editor.selectedText
            findReplaceFindField.selectAll()
            findReplaceReplaceField.text = ""
            findReplaceFindField.forceActiveFocus()
        }
    }

    Controls.Dialog {
        id: linkDialog

        title: i18nc("@title:dialog", "Insert Link")
        anchors.centerIn: parent
        modal: true
        focus: true
        width: Kirigami.Units.gridUnit * 24
        standardButtons: Controls.Dialog.Cancel
        padding: Kirigami.Units.largeSpacing

        footer: Controls.DialogButtonBox {
            alignment: Qt.AlignRight
            Controls.Button {
                text: i18nc("@action", "Insert")
                enabled: linkLabelField.text.trim().length > 0 && linkUrlField.text.trim().length > 0
                onClicked: {
                    editorPane.insertLink(linkLabelField.text.trim(), linkUrlField.text.trim())
                    linkDialog.close()
                }
            }
        }

        contentItem: GridLayout {
            columns: 2
            rowSpacing: Kirigami.Units.smallSpacing
            columnSpacing: Kirigami.Units.largeSpacing

            Controls.Label { text: i18nc("@label", "Text") }
            Controls.TextField {
                id: linkLabelField
                Layout.fillWidth: true
                text: editor.selectedText.length > 0 ? editor.selectedText : "link text"
            }

            Controls.Label { text: i18nc("@label", "URL") }
            Controls.TextField {
                id: linkUrlField
                Layout.fillWidth: true
                placeholderText: "https://"
                text: "https://"
            }
        }

        onOpened: {
            linkLabelField.text = editor.selectedText.length > 0 ? editor.selectedText : "link text"
            linkUrlField.text = "https://"
            linkLabelField.forceActiveFocus()
            linkLabelField.selectAll()
        }
    }

    Controls.Dialog {
        id: figureDialog

        title: i18nc("@title:dialog", "Insert Figure")
        anchors.centerIn: parent
        modal: true
        focus: true
        width: Kirigami.Units.gridUnit * 28
        standardButtons: Controls.Dialog.Cancel
        padding: Kirigami.Units.largeSpacing

        property string imagePath: ""

        footer: Controls.DialogButtonBox {
            alignment: Qt.AlignRight
            Controls.Button {
                text: i18nc("@action", "Insert")
                enabled: figureDialog.imagePath.length > 0
                onClicked: {
                    editorPane.insertFigure(
                        figureDialog.imagePath,
                        figureAltField.text.trim(),
                        figureCaptionField.text.trim()
                    )
                    figureDialog.close()
                }
            }
        }

        contentItem: ColumnLayout {
            spacing: Kirigami.Units.smallSpacing

            RowLayout {
                Layout.fillWidth: true
                spacing: Kirigami.Units.smallSpacing

                Controls.TextField {
                    id: figurePathField
                    Layout.fillWidth: true
                    readOnly: true
                    placeholderText: i18nc("@info:placeholder", "Choose an image file…")
                }
                Controls.Button {
                    text: i18nc("@action", "Browse…")
                    onClicked: {
                        const path = document.pickImageFile()
                        if (path.length > 0) {
                            figureDialog.imagePath = path
                            figurePathField.text = path
                            if (figureAltField.text.length === 0) {
                                figureAltField.text = path.split("/").pop().split("\\").pop()
                            }
                        }
                    }
                }
            }

            GridLayout {
                Layout.fillWidth: true
                columns: 2
                rowSpacing: Kirigami.Units.smallSpacing
                columnSpacing: Kirigami.Units.largeSpacing

                Controls.Label { text: i18nc("@label", "Alt text") }
                Controls.TextField {
                    id: figureAltField
                    Layout.fillWidth: true
                    placeholderText: i18nc("@info:placeholder", "Figure description")
                }

                Controls.Label { text: i18nc("@label", "Caption") }
                Controls.TextField {
                    id: figureCaptionField
                    Layout.fillWidth: true
                    placeholderText: i18nc("@info:placeholder", "Figure caption")
                    text: "Figure caption"
                }
            }
        }

        onOpened: {
            imagePath = ""
            figurePathField.text = ""
            figureAltField.text = ""
            figureCaptionField.text = "Figure caption"
        }
    }
}
