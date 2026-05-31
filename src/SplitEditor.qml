import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import QtWebEngine
import QtWebChannel
import org.kde.kirigami as Kirigami
import org.kde.simplemd
import "editorfonts.js" as EditorFonts

Item {
    id: root

    required property DocumentController document
    required property QtObject preferences
    required property EditorHelper editorHelper

    property alias editor: editor

    readonly property int pad: Kirigami.Units.gridUnit
    readonly property real editorPad: pad * 1.2
    readonly property real editorOnlyMaxWidth: 720

    PreviewHelper {
        id: previewHelper
    }

    PrintHelper {
        id: printHelper
    }

    property bool previewPageReady: false
    property bool scrollSyncLock: false
    property bool pendingOutput: false
    property string outputMode: ""
    property string exportPath: ""
    property var exportOptions: ({})
    property bool restorePending: false
    property bool previewHiddenForOutput: false
    property bool previewShowingLink: false
    property bool outlineVisible: false

    readonly property string previewDocumentUrl: "qrc:/preview/preview.html"

    QtObject {
        id: scrollSync

        property int lastCursorLine: 1
        property int lastAnchorLine: 1

        function sourceLineAtPosition(pos) {
            const text = editor.text.slice(0, Math.max(0, pos))
            if (text.length === 0) {
                return 1
            }
            return text.split("\n").length
        }

        function sourceLineForCursor() {
            return sourceLineAtPosition(editor.cursorPosition)
        }

        function charOffsetForLine(line) {
            let pos = 0
            const lines = editor.text.split("\n")
            const count = Math.max(0, Math.min(line - 1, lines.length))
            for (let i = 0; i < count; i++) {
                pos += lines[i].length + 1
            }
            return Math.min(pos, editor.text.length)
        }

        function sourceLineAtScrollTop() {
            const flickable = editorScroll.contentItem
            if (!flickable || editor.text.length === 0) {
                return sourceLineForCursor()
            }
            const targetY = flickable.contentY + editorPad
            let lo = 0
            let hi = editor.text.length
            while (lo < hi) {
                const mid = Math.floor((lo + hi) / 2)
                const rect = editor.positionToRectangle(mid)
                if (rect.y < targetY) {
                    lo = mid + 1
                } else {
                    hi = mid
                }
            }
            return sourceLineAtPosition(lo)
        }

        function scrollEditorToLine(line, ratio) {
            const flickable = editorScroll.contentItem
            if (!flickable) {
                return
            }
            root.scrollSyncLock = true
            const pos = charOffsetForLine(line)
            const rect = editor.positionToRectangle(pos)
            const maxY = Math.max(0, flickable.contentHeight - flickable.height)
            let targetY = rect.y - editorPad + ratio * flickable.height * 0.22
            targetY = Math.max(0, Math.min(targetY, maxY))
            flickable.contentY = targetY
            editorSyncRelease.restart()
        }

        function pushPreviewScroll(anchorLine, cursorLine) {
            if (!root.previewPageReady || root.scrollSyncLock || root.previewShowingLink) {
                return
            }
            root.scrollSyncLock = true
            previewWeb.runJavaScript(
                "typeof scrollPreviewForEditor === 'function' && scrollPreviewForEditor("
                + anchorLine + "," + cursorLine + ")"
            )
            previewSyncRelease.restart()
        }

        function syncFromEditor() {
            if (root.scrollSyncLock || !root.previewPageReady) {
                return
            }
            lastCursorLine = sourceLineForCursor()
            lastAnchorLine = sourceLineAtScrollTop()
            pushPreviewScroll(lastAnchorLine, lastCursorLine)
        }

        function onPreviewScroll(line, ratio) {
            if (root.scrollSyncLock) {
                return
            }
            lastAnchorLine = line
            scrollEditorToLine(line, ratio)
        }

        function onPreviewUpdated() {
            syncFromEditor()
        }
    }

    Timer {
        id: editorSyncRelease
        interval: 130
        repeat: false
        onTriggered: root.scrollSyncLock = false
    }

    Timer {
        id: previewSyncRelease
        interval: 130
        repeat: false
        onTriggered: root.scrollSyncLock = false
    }

    Timer {
        id: editorScrollSyncDebounce
        interval: 60
        repeat: false
        onTriggered: scrollSync.syncFromEditor()
    }

    Timer {
        id: cursorSyncDebounce
        interval: 40
        repeat: false
        onTriggered: scrollSync.syncFromEditor()
    }

    QtObject {
        id: scrollBridge
        WebChannel.id: "scrollBridge"

        function reportPreviewScroll(line, ratio) {
            scrollSync.onPreviewScroll(line, ratio)
        }

        function previewUpdated() {
            scrollSync.onPreviewUpdated()
        }

        function outputReady(mode) {
            const outputType = mode || root.outputMode
            if (outputType === "export") {
                const title = root.document.windowTitle.replace(/\s*\[\*\]\s*$/, "")
                const opts = Object.assign({}, root.exportOptions, { title: title })
                previewWeb.runJavaScript(
                    "typeof prepareForExport === 'function' && prepareForExport("
                    + JSON.stringify(opts) + ")",
                    () => { outputDelayTimer.start() }
                )
            } else {
                previewWeb.runJavaScript(
                    "typeof prepareForPrint === 'function' && prepareForPrint()",
                    () => { outputDelayTimer.start() }
                )
            }
        }

        function printReady() {
            outputReady("print")
        }
    }

    WebChannel {
        id: previewChannel
        registeredObjects: [scrollBridge]
    }

    function colorHex(c) {
        return Qt.rgba(c.r, c.g, c.b, c.a)
    }

    function colorRgba(c, alpha) {
        return "rgba("
            + Math.round(c.r * 255) + ","
            + Math.round(c.g * 255) + ","
            + Math.round(c.b * 255) + ","
            + alpha + ")"
    }

    function scrollbarThumbColor() {
        return colorRgba(Kirigami.Theme.textColor, 0.36)
    }

    function scrollbarThumbHoverColor() {
        return colorRgba(Kirigami.Theme.textColor, 0.58)
    }

    function scrollbarThumbActiveColor() {
        return colorRgba(Kirigami.Theme.textColor, 0.72)
    }

    function scrollbarTrackColor() {
        return "transparent"
    }

    function pdfPageSizeId(pageSize) {
        switch (pageSize) {
        case "Letter": return WebEngineView.Letter
        case "Legal": return WebEngineView.Legal
        default: return WebEngineView.A4
        }
    }

    function pdfOrientationId(orientation) {
        return orientation === "landscape"
            ? WebEngineView.Landscape
            : WebEngineView.Portrait
    }

    function exportTitle() {
        return root.document.windowTitle.replace(/\s*\[\*\]\s*$/, "")
    }

    Timer {
        id: outputLayoutTimer
        interval: 180
        repeat: false
        onTriggered: {
            if (root.pendingOutput) {
                root.updatePreview()
            }
        }
    }

    Timer {
        id: outputDelayTimer
        interval: 250
        repeat: false
        onTriggered: finishOutput()
    }

    function finishOutput() {
        if (root.outputMode === "export" && root.exportPath.length > 0) {
            previewWeb.backgroundColor = "white"
            const opts = root.exportOptions
            previewWeb.printToPdf(
                root.exportPath,
                pdfPageSizeId(opts.pageSize),
                pdfOrientationId(opts.orientation)
            )
        } else if (root.outputMode === "print") {
            printHelper.printPreview(previewWeb)
            restorePreviewAfterOutput()
        }
        root.pendingOutput = false
        root.outputMode = ""
    }

    function ensurePreviewForOutput() {
        if (!root.preferences.previewVisible) {
            root.previewHiddenForOutput = true
            root.preferences.previewVisible = true
        }
        setPreviewLifecycle(true)
    }

    function restorePreviewAfterOutput() {
        previewWeb.backgroundColor = Kirigami.Theme.backgroundColor
        if (root.previewHiddenForOutput) {
            root.previewHiddenForOutput = false
            root.preferences.previewVisible = false
        }
    }

    function startOutputPreview(needsLayoutDelay) {
        if (needsLayoutDelay) {
            outputLayoutTimer.restart()
        } else {
            updatePreview()
        }
    }

    function beginOutput(mode, path, options) {
        const previewWasHidden = !root.preferences.previewVisible
        ensurePreviewForOutput()

        root.outputMode = mode
        root.exportPath = path || ""
        root.exportOptions = options || {}
        root.pendingOutput = true

        if (mode === "export") {
            previewWeb.backgroundColor = "white"
        }

        if (root.previewShowingLink) {
            returnToDocumentPreview()
            return true
        }

        if (!previewPageReady) {
            checkPreviewReady()
            return true
        }

        startOutputPreview(previewWasHidden || previewHiddenForOutput)
        return true
    }

    function togglePreview() {
        root.preferences.previewVisible = !root.preferences.previewVisible
    }

    function setPreviewLifecycle(active) {
        previewWeb.lifecycleState = active
            ? WebEngineView.LifecycleState.Active
            : WebEngineView.LifecycleState.Frozen
    }

    function printDocument() {
        beginOutput("print")
    }

    function exportDocument(path, options) {
        if (!path || path.length === 0) {
            return false
        }
        return beginOutput("export", path, options)
    }

    function clearExportLayout() {
        previewWeb.runJavaScript("typeof clearExportLayout === 'function' && clearExportLayout()")
    }

    function onWindowMinimized() {
        previewWeb.lifecycleState = WebEngineView.LifecycleState.Frozen
    }

    function onWindowRestored() {
        if (restorePending) {
            return
        }
        restorePending = true
        previewWeb.lifecycleState = WebEngineView.LifecycleState.Active
        previewRestoreTimer.restart()
    }

    function isPreviewDocumentUrl(url) {
        const s = url.toString()
        return s.startsWith("qrc:") && s.includes("preview/preview.html")
    }

    function updatePreviewLinkState() {
        previewShowingLink = !isPreviewDocumentUrl(previewWeb.url)
    }

    function returnToDocumentPreview() {
        if (isPreviewDocumentUrl(previewWeb.url)) {
            return
        }
        previewShowingLink = false
        previewPageReady = false
        previewWeb.url = previewDocumentUrl
    }

    function finishPreviewRestore() {
        previewPageReady = false
        previewWeb.reload()
    }

    function updatePreview() {
        if (!previewWeb || !previewPageReady || root.previewShowingLink) {
            return
        }
        const script = previewHelper.buildPreviewScript(
            editor.text,
            colorHex(Kirigami.Theme.textColor),
            colorHex(Kirigami.Theme.backgroundColor),
            colorHex(Kirigami.Theme.linkColor),
            root.preferences.previewFontFamily,
            root.preferences.previewFontSize,
            root.preferences.previewLineHeight,
            pad * 1.8,
            46,
            root.document.documentDirectory(),
            scrollbarThumbColor(),
            scrollbarTrackColor(),
            scrollbarThumbHoverColor(),
            scrollbarThumbActiveColor()
        )
        if (script.length > 0) {
            const cursorLine = scrollSync.sourceLineForCursor()
            const anchorLine = pendingOutput ? 1 : scrollSync.sourceLineAtScrollTop()
            const ensureLine = pendingOutput ? 1 : cursorLine
            const preamble = "window._pendingOutput=" + (pendingOutput ? JSON.stringify(outputMode) : "''")
                + ";window._pendingPrint=" + (pendingOutput ? "true" : "false")
                + ";window._pendingEnsureLine=" + ensureLine
                + ";window._pendingAnchorLine=" + anchorLine + ";"
            previewWeb.runJavaScript(preamble + script)
        }
    }

    function checkPreviewReady() {
        previewWeb.runJavaScript(
            "window.previewReady && typeof marked !== 'undefined' && typeof updatePreviewFromBase64 === 'function'",
            ready => {
                previewPageReady = ready === true
                if (previewPageReady) {
                    if (root.pendingOutput && root.outputMode) {
                        root.startOutputPreview(root.previewHiddenForOutput)
                    } else {
                        root.updatePreview()
                    }
                    if (root.restorePending) {
                        root.restorePending = false
                    }
                } else {
                    previewInitTimer.start()
                }
            }
        )
    }

    function findNext(term, backward) {
        if (!term || term.length === 0) {
            return false
        }
        const haystack = root.document.text
        const from = backward
            ? (editor.selectionStart > 0 ? editor.selectionStart - 1 : haystack.length - 1)
            : editor.selectionEnd
        const idx = backward
            ? haystack.lastIndexOf(term, from)
            : haystack.indexOf(term, from)
        if (idx < 0) {
            return false
        }
        editor.textEdit.select(idx, idx + term.length)
        editor.textEdit.forceActiveFocus()
        return true
    }

    function replaceNext(term, replacement, backward) {
        if (!term || term.length === 0) {
            return false
        }
        if (!findNext(term, backward)) {
            return false
        }
        const from = editor.textEdit.selectionStart
        const to = editor.textEdit.selectionEnd
        editor.textEdit.remove(from, to)
        editor.textEdit.insert(from, replacement)
        editor.textEdit.select(from, from + replacement.length)
        editor.textEdit.forceActiveFocus()
        return true
    }

    function replaceAll(term, replacement) {
        if (!term || term.length === 0) {
            return 0
        }
        let text = root.document.text
        let count = 0
        let idx = 0
        while ((idx = text.indexOf(term, idx)) >= 0) {
            text = text.slice(0, idx) + replacement + text.slice(idx + term.length)
            idx += replacement.length
            count += 1
        }
        if (count > 0) {
            root.document.text = text
            editor.textEdit.forceActiveFocus()
        }
        return count
    }

    function openDroppedDocument(path) {
        if (!path || path.length === 0) {
            return
        }
        if (!root.document.canClose()) {
            return
        }
        root.document.openFile(path)
    }

    function handleFileDrop(paths) {
        if (!paths || paths.length === 0) {
            return
        }

        let openedDocument = false
        for (let i = 0; i < paths.length; ++i) {
            const path = paths[i]
            if (!path || path.length === 0) {
                continue
            }

            const dot = path.lastIndexOf(".")
            const ext = dot >= 0 ? path.substring(dot + 1).toLowerCase() : ""
            const isImage = ext === "png" || ext === "jpg" || ext === "jpeg"
                || ext === "gif" || ext === "svg" || ext === "webp" || ext === "bmp"
            const isDocument = dot < 0 || ext === "md" || ext === "markdown"
                || ext === "mdown" || ext === "txt"

            if (isImage) {
                root.insertImage(path)
                continue
            }

            if (!openedDocument && isDocument) {
                root.openDroppedDocument(path)
                openedDocument = true
            }
        }
    }

    function selectAll() {
        editor.selectAll()
        editor.forceActiveFocus()
    }

    function selectedTextForAi() {
        return editor.selectedText
    }

    function beforeCursorContext(maxChars) {
        const end = Math.max(0, editor.cursorPosition)
        const start = Math.max(0, end - maxChars)
        return editor.text.slice(start, end)
    }

    function afterCursorContext(maxChars) {
        const start = Math.max(0, editor.cursorPosition)
        const end = Math.min(editor.text.length, start + maxChars)
        return editor.text.slice(start, end)
    }

    function applyAiResult(text) {
        if (!text || text.length === 0) {
            return
        }

        if (editor.selectedText.length > 0) {
            const from = Math.min(editor.selectionStart, editor.selectionEnd)
            const to = Math.max(editor.selectionStart, editor.selectionEnd)
            editor.remove(from, to)
            editor.insert(from, text)
            editor.cursorPosition = from + text.length
        } else {
            const pos = editor.cursorPosition
            editor.insert(pos, text)
            editor.cursorPosition = pos + text.length
        }

        editor.forceActiveFocus()
    }

    function insertAt(pos, text, selectStart, selectLength) {
        editor.insert(pos, text)
        if (selectLength > 0) {
            editor.select(pos + selectStart, pos + selectStart + selectLength)
        } else {
            editor.cursorPosition = pos + text.length
        }
        editor.forceActiveFocus()
    }

    function insertInline(before, after, placeholder) {
        const inner = placeholder || ""
        if (editor.selectedText.length > 0) {
            const from = Math.min(editor.selectionStart, editor.selectionEnd)
            const to = Math.max(editor.selectionStart, editor.selectionEnd)
            const selected = editor.selectedText
            editor.remove(from, to)
            insertAt(from, before + selected + after, before.length, selected.length)
        } else {
            insertAt(editor.cursorPosition, before + inner + after, before.length, inner.length)
        }
    }

    function insertBlock(text, selectStart, selectLength) {
        let pos = editor.cursorPosition
        let block = text
        const before = editor.text.slice(0, pos)
        if (pos > 0 && !before.endsWith("\n")) {
            block = "\n" + block
            pos += 1
        }
        if (!block.endsWith("\n")) {
            block += "\n"
        }
        insertAt(pos, block, selectStart, selectLength)
    }

    function formatCurrentDate() {
        const d = new Date()
        const y = d.getFullYear()
        const m = String(d.getMonth() + 1).padStart(2, "0")
        const day = String(d.getDate()).padStart(2, "0")
        return y + "/" + m + "/" + day
    }

    function insertAuthor(authorName) {
        const name = (authorName && authorName.length > 0) ? authorName : "Author Name"
        const line = "**Author:** " + name + "\n\n"
        insertBlock(line, 11, name.length)
    }

    function insertDate() {
        const date = formatCurrentDate()
        const line = date + "\n\n"
        insertBlock(line, 0, 0)
    }

    function insertAuthorAndDate(authorName) {
        const name = (authorName && authorName.length > 0) ? authorName : "Author Name"
        const date = formatCurrentDate()
        const line = "**Author:** " + name + "\n" + date + "\n\n"
        insertBlock(line, 11, name.length)
    }

    function insertImage(path, altText) {
        if (!path || path.length === 0) {
            return
        }
        const mdPath = root.document.markdownPath(path)
        let alt = altText
        if (!alt || alt.length === 0) {
            const slash = Math.max(path.lastIndexOf("/"), path.lastIndexOf("\\"))
            const name = slash >= 0 ? path.substring(slash + 1) : path
            const dot = name.lastIndexOf(".")
            alt = dot >= 0 ? name.substring(0, dot) : name
        }
        if (alt.length === 0) {
            alt = "Image description"
        }
        insertBlock("![" + alt + "](" + mdPath + ")\n", 2, alt.length)
    }

    function insertFigure(path, altText, caption) {
        if (!path || path.length === 0) {
            return
        }
        const mdPath = root.document.markdownPath(path)
        const alt = (altText || "Figure").replace(/&/g, "&amp;").replace(/"/g, "&quot;")
        const cap = (caption || "Figure caption").replace(/&/g, "&amp;").replace(/</g, "&lt;").replace(/>/g, "&gt;")
        const block = "<figure>\n  <img src=\"" + mdPath + "\" alt=\"" + alt + "\">\n  <figcaption>" + cap + "</figcaption>\n</figure>\n\n"
        insertBlock(block, block.indexOf(alt), alt.length)
    }

    function insertMermaid(source) {
        insertBlock("```mermaid\n" + source + "\n```\n\n", 11, 0)
    }

    function wrapSelection(before, after) {
        insertInline(before, after, "text")
    }

    function insertHeading(level, title) {
        const hashes = "#".repeat(Math.max(1, Math.min(level, 6)))
        const label = title || "Heading"
        insertBlock(hashes + " " + label + "\n\n", hashes.length + 1, label.length)
    }

    function insertLink(label, url) {
        const text = label || "link text"
        const href = url || "https://"
        insertInline("[", "](" + href + ")", text)
    }

    function insertFootnoteReference(label) {
        const id = label || "note"
        insertInline("[^", "]", id)
    }

    function insertFootnoteDefinition(label, body) {
        const id = label || "note"
        const content = body || "Reference details."
        insertBlock("[^" + id + "]: " + content + "\n\n", 4 + id.length + 2, content.length)
    }

    function insertBibliographyEntry(key, body) {
        const id = key || "Author2024"
        const content = body || "Author, *Title*, Publisher, 2024."
        insertBlock("[^" + id + "]: " + content + "\n\n", 4 + id.length + 2, content.length)
    }

    function insertChemicalFormula(formula) {
        const sample = formula || "H2O"
        insertInline("$\\ce{", "}$", sample)
    }

    function insertChemicalEquation(equation) {
        const sample = equation || "2H2 + O2 -> 2H2O"
        insertBlock("$$\n\\ce{" + sample + "}\n$$\n\n", 7, sample.length)
    }

    function insertPhysicalUnit(unit) {
        const sample = unit || "mol/L"
        insertInline("$\\pu{", "}$", sample)
    }

    function toggleDocumentOutline() {
        root.outlineVisible = !root.outlineVisible
        if (root.outlineVisible) {
            outlinePanel.focusList()
        } else {
            editor.forceActiveFocus()
        }
    }

    function goToOutlineLine(line) {
        const pos = scrollSync.charOffsetForLine(line)
        editor.cursorPosition = pos
        editor.select(pos, pos)
        editor.forceActiveFocus()
        Qt.callLater(() => scrollSync.scrollEditorToLine(line, 0))
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        DocumentOutlinePanel {
            id: outlinePanel

            visible: root.outlineVisible
            Layout.preferredWidth: visible ? implicitWidth : 0
            Layout.fillHeight: true
            documentText: editor.text
            cursorLine: scrollSync.sourceLineForCursor()

            onNavigateToLine: line => root.goToOutlineLine(line)
            onCloseRequested: {
                root.outlineVisible = false
                editor.forceActiveFocus()
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.preferredWidth: root.preferences.previewVisible ? root.width / 2 : root.width
            spacing: 0

        Controls.ScrollView {
            id: editorScroll

            Layout.fillWidth: true
            Layout.fillHeight: true

            clip: true

            Controls.ScrollBar.vertical: StyledScrollBar {
                onPositionChanged: {
                    if (!root.scrollSyncLock && editor.textEdit.activeFocus) {
                        editorScrollSyncDebounce.restart()
                    }
                }
            }
            Controls.ScrollBar.horizontal: StyledScrollBar {}

            MarkdownEditor {
                id: editor

                width: editorScroll.availableWidth
                scrollFlickable: editorScroll.contentItem
                editorHelper: root.editorHelper
                documentController: root.document
                pasteImageHandler: path => root.insertImage(path)

                readonly property real sidePadding: {
                    if (root.preferences.previewVisible) {
                        return 0
                    }
                    const innerAvail = editorScroll.availableWidth - 2 * editorPad
                    const textColumn = Math.min(root.editorOnlyMaxWidth, innerAvail - root.pad * 8)
                    return Math.max(0, (innerAvail - textColumn) / 2)
                }

                showWhitespace: root.preferences.showWhitespace
                syntaxHighlighting: root.preferences.syntaxHighlighting
                wrapMode: TextEdit.Wrap
                selectByMouse: true
                font.family: EditorFonts.resolveFamily(
                    root.preferences.editorFontFamily,
                    Kirigami.Theme.fixedFont ? Kirigami.Theme.fixedFont.family : "")
                font.pixelSize: root.preferences.editorFontSize
                color: Kirigami.Theme.textColor
                selectionColor: Kirigami.Theme.highlightColor
                selectedTextColor: Kirigami.Theme.highlightedTextColor
                leftPadding: editorPad + sidePadding
                rightPadding: editorPad + sidePadding
                topPadding: editorPad
                bottomPadding: editorPad

                onTextChanged: {
                    if (root.document.text !== text) {
                        root.document.text = text
                    }
                }

                onCursorPositionChanged: {
                    cursorSyncDebounce.restart()
                    outlinePanel.refresh()
                }

                Component.onCompleted: {
                    if (text !== root.document.text) {
                        text = root.document.text
                    }
                }

                Connections {
                    target: root.document
                    function onTextChanged() {
                        if (editor.text !== root.document.text) {
                            editor.text = root.document.text
                        }
                    }
                }
            }
        }

        WheelHandler {
            target: editor.textEdit
            onWheel: {
                if (!root.scrollSyncLock) {
                    editorScrollSyncDebounce.restart()
                }
            }
        }

        }

        Rectangle {
            id: previewDivider

            visible: root.preferences.previewVisible
            Layout.preferredWidth: visible ? 1 : 0
            Layout.fillHeight: true
            color: Qt.rgba(Kirigami.Theme.textColor.r,
                           Kirigami.Theme.textColor.g,
                           Kirigami.Theme.textColor.b,
                           0.08)
        }

        ColumnLayout {
            id: previewColumn

            visible: root.preferences.previewVisible
            Layout.fillWidth: root.preferences.previewVisible
            Layout.fillHeight: true
            Layout.preferredWidth: root.preferences.previewVisible ? root.width / 2 : 0
            spacing: 0

            Rectangle {
                visible: root.previewShowingLink
                Layout.fillWidth: true
                Layout.preferredHeight: visible ? previewLinkBar.implicitHeight + root.pad * 0.4 : 0
                color: Kirigami.Theme.backgroundColor

                Rectangle {
                    anchors.bottom: parent.bottom
                    width: parent.width
                    height: 1
                    color: Qt.rgba(Kirigami.Theme.textColor.r,
                                   Kirigami.Theme.textColor.g,
                                   Kirigami.Theme.textColor.b,
                                   0.08)
                }

                RowLayout {
                    id: previewLinkBar

                    anchors.fill: parent
                    anchors.leftMargin: root.pad * 0.3
                    anchors.rightMargin: root.pad * 0.3
                    spacing: root.pad * 0.3

                    Controls.ToolButton {
                        icon.name: "window-close-symbolic"
                        display: Controls.AbstractButton.IconOnly
                        flat: true
                        Layout.preferredWidth: Kirigami.Units.gridUnit * 2.4
                        Layout.preferredHeight: Kirigami.Units.gridUnit * 2.4
                        opacity: hovered ? 1.0 : 0.72
                        Controls.ToolTip.visible: hovered
                        Controls.ToolTip.text: i18nc("@action", "Back to document preview")
                        onClicked: root.returnToDocumentPreview()
                    }

                    Controls.Label {
                        Layout.fillWidth: true
                        text: previewWeb.url.toString()
                        elide: Text.ElideMiddle
                        opacity: 0.72
                    }
                }
            }

            WebEngineView {
                id: previewWeb

                Layout.fillWidth: true
                Layout.fillHeight: true

                url: root.previewDocumentUrl
                webChannel: previewChannel
                backgroundColor: Kirigami.Theme.backgroundColor
                lifecycleState: root.preferences.previewVisible
                    ? WebEngineView.LifecycleState.Active
                    : WebEngineView.LifecycleState.Frozen
                settings.localContentCanAccessRemoteUrls: false
                settings.localContentCanAccessFileUrls: true
                settings.javascriptEnabled: true
                settings.preferCSSMarginsForPrinting: true

                onUrlChanged: root.updatePreviewLinkState()

                onLoadingChanged: loadRequest => {
                    if (loadRequest.status === WebEngineView.LoadSucceededStatus) {
                        root.checkPreviewReady()
                    }
                }

                onJavaScriptConsoleMessage: (level, line, message) => {
                    if (level === WebEngineView.ErrorMessageLevel) {
                        console.warn("Preview:", line, message)
                    }
                }

                onPdfPrintingFinished: (filePath, success) => {
                    if (root.exportPath.length > 0 && filePath === root.exportPath) {
                        root.clearExportLayout()
                        root.exportPath = ""
                        root.restorePreviewAfterOutput()
                    }
                    if (!success) {
                        console.warn("PDF output failed:", filePath)
                    }
                }
            }
        }

        Connections {
            target: root.preferences
            function onPreviewVisibleChanged() {
                if (root.pendingOutput || root.previewHiddenForOutput) {
                    return
                }
                setPreviewLifecycle(root.preferences.previewVisible)
            }
        }

        Timer {
            id: previewRestoreTimer
            interval: 120
            repeat: false
            onTriggered: root.finishPreviewRestore()
        }

        Timer {
            id: previewInitTimer
            interval: 80
            repeat: false
            onTriggered: root.checkPreviewReady()
        }

        Timer {
            id: previewDebounce
            interval: Math.min(400, 150 + Math.floor(editor.text.length / 8000))
            repeat: false
            onTriggered: root.updatePreview()
        }

        Connections {
            target: editor
            function onTextChanged() {
                previewDebounce.restart()
            }
        }

        Connections {
            target: root.preferences
            function onPreviewFontFamilyChanged() { previewDebounce.restart() }
            function onPreviewFontSizeChanged() { previewDebounce.restart() }
            function onPreviewLineHeightChanged() { previewDebounce.restart() }
        }

        Connections {
            target: root.document
            function onFilePathChanged() { previewDebounce.restart() }
        }
    }
}
