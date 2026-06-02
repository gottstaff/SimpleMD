import QtQuick
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami
import io.github.gottstaff.SimpleMD
import "editorfonts.js" as EditorFonts

Item {
    id: root

    property EditorHelper editorHelper: null
    property var documentController: null
    property var pasteImageHandler: null

    property alias text: textArea.text
    property alias font: textArea.font
    property alias color: textArea.color
    property alias wrapMode: textArea.wrapMode
    property alias readOnly: textArea.readOnly
    property alias selectByMouse: textArea.selectByMouse
    property alias leftPadding: textArea.leftPadding
    property alias rightPadding: textArea.rightPadding
    property alias topPadding: textArea.topPadding
    property alias bottomPadding: textArea.bottomPadding
    property alias selectionColor: textArea.selectionColor
    property alias selectedTextColor: textArea.selectedTextColor
    property alias cursorPosition: textArea.cursorPosition
    property alias selectionStart: textArea.selectionStart
    property alias selectionEnd: textArea.selectionEnd
    property alias selectedText: textArea.selectedText
    property alias canUndo: textArea.canUndo
    property alias canRedo: textArea.canRedo
    property alias canPaste: textArea.canPaste
    property alias textEdit: textArea

    property bool showWhitespace: false
    property bool syntaxHighlighting: false
    property var scrollFlickable: null

    readonly property color syntaxHeadingColor: Qt.rgba(
        textArea.color.r, textArea.color.g, textArea.color.b, 0.9)
    readonly property color syntaxMarkerColor: Qt.rgba(
        textArea.color.r, textArea.color.g, textArea.color.b, 0.38)
    readonly property color syntaxCodeColor: Qt.rgba(
        Kirigami.Theme.linkColor.r,
        Kirigami.Theme.linkColor.g,
        Kirigami.Theme.linkColor.b,
        0.72)
    readonly property color syntaxLinkColor: Qt.rgba(
        Kirigami.Theme.linkColor.r,
        Kirigami.Theme.linkColor.g,
        Kirigami.Theme.linkColor.b,
        0.62)
    readonly property color syntaxEmphasisColor: Qt.rgba(
        textArea.color.r, textArea.color.g, textArea.color.b, 0.76)

    readonly property color whitespaceColor: Qt.rgba(
        textArea.color.r, textArea.color.g, textArea.color.b, 0.38)
    readonly property color bracketMatchColor: Qt.rgba(
        Kirigami.Theme.highlightColor.r,
        Kirigami.Theme.highlightColor.g,
        Kirigami.Theme.highlightColor.b,
        0.42)
    readonly property color currentLineColor: Qt.tint(
        Kirigami.Theme.backgroundColor,
        Qt.rgba(Kirigami.Theme.textColor.r,
                Kirigami.Theme.textColor.g,
                Kirigami.Theme.textColor.b,
                0.07))

    implicitWidth: textArea.implicitWidth
    implicitHeight: textArea.implicitHeight
    height: textArea.height

    function undo() { textArea.undo() }
    function redo() { textArea.redo() }
    function cut() { textArea.cut() }
    function copy() { textArea.copy() }
    function paste() {
        if (documentController && documentController.clipboardHasImage()) {
            const path = documentController.pasteImageFromClipboard()
            if (path.length > 0 && pasteImageHandler) {
                pasteImageHandler(path)
            }
            return
        }
        textArea.paste()
    }
    function selectAll() { textArea.selectAll() }
    function select(start, end) { textArea.select(start, end) }
    function insert(pos, chunk) { textArea.insert(pos, chunk) }
    function remove(start, end) { textArea.remove(start, end) }
    function forceActiveFocus(reason) { textArea.forceActiveFocus(reason) }
    function positionToRectangle(pos) { return textArea.positionToRectangle(pos) }

    function lineStartForPosition(pos) {
        const index = Math.max(0, Math.min(pos, textArea.text.length))
        const newline = textArea.text.lastIndexOf("\n", index - 1)
        return newline < 0 ? 0 : newline + 1
    }

    function lineEndForPosition(lineStart) {
        const next = textArea.text.indexOf("\n", lineStart)
        return next < 0 ? textArea.text.length : next
    }

    function isOnFirstLine() {
        return lineStartForPosition(textArea.cursorPosition) === 0
    }

    function ensureCursorVisible() {
        const flickable = root.scrollFlickable
        if (!flickable) {
            return
        }

        const rect = textArea.cursorRectangle
        const tp = textArea.topPadding
        const bp = textArea.bottomPadding
        const margin = Math.max(6, fontMetrics.height * 0.2)
        const viewTop = flickable.contentY + tp
        const viewBottom = flickable.contentY + flickable.height - bp
        let targetY = flickable.contentY

        if (rect.bottom + margin > viewBottom) {
            targetY = rect.bottom - flickable.height + bp + margin
        } else if (rect.top - margin < viewTop) {
            targetY = rect.top - tp - margin
        } else {
            return
        }

        const maxY = Math.max(0, flickable.contentHeight - flickable.height)
        flickable.contentY = Math.max(0, Math.min(targetY, maxY))
    }

    function scheduleCurrentLineHighlightUpdate() {
        lineHighlightTimer.restart()
    }

    function updateCurrentLineHighlight() {
        if (!textArea.activeFocus) {
            currentLineHighlight.visible = false
            return
        }

        const lineStart = lineStartForPosition(textArea.cursorPosition)
        const lineEnd = lineEndForPosition(lineStart)
        const startRect = textArea.positionToRectangle(lineStart)
        const cursorRect = textArea.cursorRectangle

        let top = Math.min(startRect.y, cursorRect.y)
        let bottom = Math.max(startRect.y + startRect.height, cursorRect.y + cursorRect.height)

        if (lineEnd > lineStart) {
            const endRect = textArea.positionToRectangle(lineEnd - 1)
            top = Math.min(top, endRect.y)
            bottom = Math.max(bottom, endRect.y + endRect.height)
        }

        currentLineHighlight.y = top
        currentLineHighlight.height = Math.max(bottom - top, fontMetrics.height)
        currentLineHighlight.visible = true
    }

    FontMetrics {
        id: fontMetrics
        font: textArea.font
    }

    Timer {
        id: lineHighlightTimer
        interval: 0
        repeat: false
        onTriggered: root.updateCurrentLineHighlight()
    }

    Timer {
        id: whitespaceRefreshTimer
        interval: 80
        repeat: false
        onTriggered: {
            if (root.showWhitespace) {
                whitespaceCanvas.requestPaint()
            }
        }
    }

    Rectangle {
        id: currentLineHighlight

        z: 0
        width: textArea.width
        x: 0
        visible: false
        color: root.currentLineColor
    }

    Controls.TextArea {
        id: textArea

        z: 1
        width: root.width > 0 ? root.width : implicitWidth
        height: implicitHeight
        background: null

        onTextChanged: {
            whitespaceRefreshTimer.restart()
            root.ensureCursorVisible()
            root.scheduleCurrentLineHighlightUpdate()
        }
        onWidthChanged: {
            whitespaceRefreshTimer.restart()
            root.scheduleCurrentLineHighlightUpdate()
        }
        onHeightChanged: {
            whitespaceRefreshTimer.restart()
            root.scheduleCurrentLineHighlightUpdate()
        }
        onCursorPositionChanged: {
            root.ensureCursorVisible()
            root.scheduleCurrentLineHighlightUpdate()
        }
        onCursorRectangleChanged: {
            root.ensureCursorVisible()
            root.scheduleCurrentLineHighlightUpdate()
        }
        onSelectionStartChanged: root.scheduleCurrentLineHighlightUpdate()
        onSelectionEndChanged: root.scheduleCurrentLineHighlightUpdate()
        onActiveFocusChanged: root.scheduleCurrentLineHighlightUpdate()

        Keys.onPressed: event => {
            if (event.key === Qt.Key_Up
                    && !(event.modifiers & (Qt.ControlModifier | Qt.AltModifier | Qt.MetaModifier))) {
                if (root.isOnFirstLine() && cursorPosition > 0) {
                    cursorPosition = 0
                    event.accepted = true
                    return
                }
            }
            if ((event.modifiers & Qt.ControlModifier) && event.key === Qt.Key_V) {
                if (root.documentController && root.documentController.clipboardHasImage()) {
                    root.paste()
                    event.accepted = true
                }
            }
        }
    }

    Canvas {
        id: whitespaceCanvas

        z: 1
        width: textArea.width
        height: textArea.height
        visible: root.showWhitespace
        enabled: false
        renderTarget: Canvas.FramebufferObject

        onPaint: {
            const ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)

            const text = textArea.text
            if (text.length === 0) {
                return
            }

            ctx.fillStyle = root.whitespaceColor
            ctx.font = EditorFonts.canvasFontSpec(
                textArea.font.pixelSize,
                fontMetrics.font.family,
                Kirigami.Theme.fixedFont ? Kirigami.Theme.fixedFont.family : "")
            ctx.textBaseline = "middle"

            const maxMarks = 2500
            const scanLimit = Math.min(text.length, 120000)
            let marks = 0

            for (let i = 0; i < scanLimit && marks < maxMarks; ++i) {
                const ch = text[i]
                let glyph = ""
                if (ch === " ") {
                    glyph = "·"
                } else if (ch === "\t") {
                    glyph = "→"
                } else if (ch === "\n") {
                    glyph = "¶"
                } else {
                    continue
                }

                const rect = textArea.positionToRectangle(i)
                if (rect.width <= 0 && ch !== "\n") {
                    continue
                }

                const x = rect.x + (ch === " " ? rect.width * 0.5 : rect.width * 0.35)
                const y = rect.y + rect.height * 0.55
                ctx.fillText(glyph, x, y)
                marks += 1
            }
        }
    }

    function attachBracketMatcher() {
        if (root.editorHelper) {
            root.editorHelper.attachBracketMatcher(textArea, root.bracketMatchColor)
            applySyntaxHighlighting()
        }
    }

    function applySyntaxHighlighting() {
        if (!root.editorHelper) {
            return
        }
        root.editorHelper.configureSyntaxHighlighting(
            root.syntaxHighlighting,
            root.syntaxHeadingColor,
            root.syntaxMarkerColor,
            root.syntaxCodeColor,
            root.syntaxLinkColor,
            root.syntaxEmphasisColor)
    }

    onSyntaxHighlightingChanged: applySyntaxHighlighting()

    Component.onCompleted: {
        attachBracketMatcher()
        updateCurrentLineHighlight()
    }

    Connections {
        target: Kirigami.Theme
        function onHighlightColorChanged() {
            root.attachBracketMatcher()
        }
        function onTextColorChanged() {
            applySyntaxHighlighting()
        }
        function onLinkColorChanged() {
            applySyntaxHighlighting()
        }
    }
}
