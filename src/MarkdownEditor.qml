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
    property bool cursorScrollLock: false

    readonly property real textLeadInset: Math.max(8, fontMetrics.ascent + 6)
    readonly property real textTrailInset: Math.max(6, fontMetrics.descent + 4)
    readonly property real lineSnapInset: Math.max(4, fontMetrics.height * 0.2)

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

    readonly property real documentHeight: {
        const _layoutWidth = textArea.width
        const tailPad = 4
        if (textArea.text.length === 0) {
            return textLeadInset + fontMetrics.height + tailPad
        }

        const len = textArea.text.length
        const endRect = textArea.positionToRectangle(len)
        let bottom = endRect.y + endRect.height
        if (len > 0) {
            const lastRect = textArea.positionToRectangle(len - 1)
            bottom = Math.max(bottom, lastRect.y + lastRect.height)
        }
        return textLeadInset + bottom + tailPad
    }

    width: textArea.width
    implicitWidth: textArea.implicitWidth
    implicitHeight: documentHeight
    height: documentHeight

    function documentScrollHeight() {
        return documentHeight
    }

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

    function contentYMax() {
        const flickable = root.scrollFlickable
        if (!flickable) {
            return 0
        }
        return Math.max(0, flickable.contentHeight - flickable.height)
    }

    function resetViewportScroll() {
        const flickable = root.scrollFlickable
        if (!flickable) {
            return
        }
        flickable.contentY = 0
    }

    function contentYForPosition(pos) {
        return textLeadInset + textArea.positionToRectangle(pos).y
    }

    function rowBoundsAtContentY(contentY) {
        if (textArea.text.length === 0) {
            const h = fontMetrics.height
            return { y: textLeadInset, height: h, bottom: textLeadInset + h }
        }

        const target = contentY
        let lo = 0
        let hi = textArea.text.length
        while (lo < hi) {
            const mid = Math.floor((lo + hi) / 2)
            if (contentYForPosition(mid) < target) {
                lo = mid + 1
            } else {
                hi = mid
            }
        }

        const pos = Math.min(lo, textArea.text.length - 1)
        const rect = textArea.positionToRectangle(pos)
        const y = textLeadInset + rect.y
        return { y: y, height: rect.height, bottom: y + rect.height }
    }

    function cursorLineBounds() {
        const pos = textArea.cursorPosition
        const lineStart = lineStartForPosition(pos)
        const cursorRect = textArea.cursorRectangle
        const startRect = textArea.positionToRectangle(lineStart)
        const atCursor = textArea.positionToRectangle(pos)

        let top = Math.min(startRect.y, cursorRect.y, atCursor.y)
        let bottom = Math.max(
            startRect.y + startRect.height,
            cursorRect.y + cursorRect.height,
            atCursor.y + atCursor.height)

        const lineEnd = lineEndForPosition(lineStart)
        if (lineEnd > lineStart) {
            const endRect = textArea.positionToRectangle(lineEnd - 1)
            top = Math.min(top, endRect.y)
            bottom = Math.max(bottom, endRect.y + endRect.height)
        }

        return {
            top: textLeadInset + top,
            bottom: textLeadInset + bottom,
            height: bottom - top
        }
    }

    function rowSpansBoundary(boundaryY, row) {
        return row.y < boundaryY - 0.5 && row.bottom > boundaryY + 0.5
    }

    function snapScrollToWholeLines() {
        const flickable = root.scrollFlickable
        if (!flickable || root.cursorScrollLock || textArea.activeFocus) {
            return
        }

        const viewH = flickable.height
        const inset = root.lineSnapInset
        const maxY = contentYMax()
        let y = Math.max(0, Math.min(flickable.contentY, maxY))

        if (textArea.text.length === 0 || viewH < inset * 2 + 4) {
            if (Math.abs(flickable.contentY - y) > 0.5) {
                flickable.contentY = y
            }
            return
        }

        const topRowAtView = rowBoundsAtContentY(y + inset + 0.5)
        if (topRowAtView.y <= textLeadInset + 1) {
            flickable.contentY = 0
            return
        }

        for (let pass = 0; pass < 6; ++pass) {
            const topEdge = y + inset
            const bottomEdge = y + viewH - inset
            const topRow = rowBoundsAtContentY(topEdge + 0.5)
            const bottomRow = rowBoundsAtContentY(Math.max(topEdge + 1, bottomEdge - 0.5))
            const topPartial = rowSpansBoundary(topEdge, topRow)
            const bottomPartial = rowSpansBoundary(bottomEdge, bottomRow)
            let nextY = y

            if (topPartial) {
                nextY = Math.max(0, topRow.y - inset)
            } else if (bottomPartial) {
                nextY = bottomRow.height >= viewH - inset * 2 - 1
                        ? Math.max(0, bottomRow.y - inset)
                        : bottomRow.y - viewH + inset
            } else {
                break
            }

            nextY = Math.max(0, Math.min(nextY, maxY))
            if (Math.abs(nextY - y) <= 0.5) {
                break
            }
            y = nextY
        }

        if (Math.abs(flickable.contentY - y) > 0.5) {
            flickable.contentY = y
        }
    }

    function scrollByWheelNotches(notches) {
        const flickable = root.scrollFlickable
        if (!flickable || notches === 0) {
            return
        }

        const lineStep = Math.max(1, Math.round(Math.abs(notches)))
        const direction = notches > 0 ? -1 : 1
        let y = flickable.contentY

        for (let i = 0; i < lineStep; ++i) {
            const row = rowBoundsAtContentY(Math.max(0, y + lineSnapInset + 0.5))
            y += direction * row.height
        }

        flickable.contentY = Math.max(0, Math.min(y, contentYMax()))
        snapScrollToWholeLines()
    }

    function ensureCursorVisible() {
        const flickable = root.scrollFlickable
        if (!flickable) {
            return
        }

        const line = cursorLineBounds()
        if (line.height <= 0) {
            return
        }

        const topMargin = Math.max(lineSnapInset, fontMetrics.ascent * 0.5 + 2)
        const bottomMargin = Math.max(lineSnapInset, fontMetrics.height * 0.3)
        const maxY = contentYMax()
        let targetY = flickable.contentY

        for (let pass = 0; pass < 2; ++pass) {
            const viewTop = targetY + topMargin
            const viewBottom = targetY + flickable.height - bottomMargin
            let changed = false

            if (line.bottom > viewBottom + 0.5) {
                targetY = line.bottom - flickable.height + bottomMargin
                changed = true
            }
            if (line.top < viewTop - 0.5) {
                targetY = line.top - topMargin
                changed = true
            }
            if (!changed) {
                break
            }
        }

        targetY = Math.max(0, Math.min(targetY, maxY))
        if (Math.abs(targetY - flickable.contentY) <= 0.5) {
            return
        }

        root.cursorScrollLock = true
        flickable.contentY = targetY
        cursorScrollRelease.restart()
    }

    function ensureRangeVisible(start, end) {
        const flickable = root.scrollFlickable
        if (!flickable) {
            return
        }

        const len = textArea.text.length
        const startPos = Math.max(0, Math.min(start, len))
        const endPos = Math.max(startPos, Math.min(end, len))
        const startRect = textArea.positionToRectangle(startPos)
        const endRect = textArea.positionToRectangle(endPos > startPos ? endPos - 1 : startPos)

        const inset = lineSnapInset
        const viewTextTop = flickable.contentY + inset
        const viewTextBottom = flickable.contentY + flickable.height - inset

        const top = textLeadInset + Math.min(startRect.y, endRect.y)
        const bottom = textLeadInset + Math.max(startRect.y + startRect.height, endRect.y + endRect.height)
        let targetY = flickable.contentY

        if (bottom > viewTextBottom) {
            targetY = bottom - flickable.height + inset
        } else if (top < viewTextTop) {
            targetY = top - inset
        } else {
            return
        }

        flickable.contentY = Math.max(0, Math.min(targetY, contentYMax()))
    }

    function scheduleCurrentLineHighlightUpdate() {
        lineHighlightTimer.restart()
    }

    function updateCurrentLineHighlight() {
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

        currentLineHighlight.y = textLeadInset + top
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
        id: cursorVisibleTimer
        interval: 0
        repeat: false
        onTriggered: root.ensureCursorVisible()
    }

    Timer {
        id: cursorScrollRelease
        interval: 120
        repeat: false
        onTriggered: root.cursorScrollLock = false
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

    TextEdit {
        id: textArea

        z: 1
        y: root.textLeadInset
        width: root.width > 0 ? root.width : implicitWidth
        height: implicitHeight
        topPadding: 0
        bottomPadding: 0

        onTextChanged: {
            whitespaceRefreshTimer.restart()
            root.scheduleCurrentLineHighlightUpdate()
            if (activeFocus) {
                cursorVisibleTimer.restart()
            }
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
            cursorVisibleTimer.restart()
            root.scheduleCurrentLineHighlightUpdate()
        }
        onCursorRectangleChanged: root.scheduleCurrentLineHighlightUpdate()
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
        y: root.textLeadInset
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

    onTextLeadInsetChanged: scheduleCurrentLineHighlightUpdate()
    onLineSnapInsetChanged: scheduleCurrentLineHighlightUpdate()

    Component.onCompleted: {
        attachBracketMatcher()
        Qt.callLater(() => {
            root.resetViewportScroll()
            root.updateCurrentLineHighlight()
        })
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
