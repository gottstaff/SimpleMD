import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts
import org.kde.kirigami as Kirigami
import "documentoutline.js" as DocumentOutline

Item {
    id: root

    property string documentText: ""
    property int cursorLine: 1
    property int activeIndex: -1

    signal navigateToLine(int line)
    signal closeRequested()

    readonly property int panelWidth: Kirigami.Units.gridUnit * 14
    readonly property var outlineItems: DocumentOutline.parseOutline(documentText)

    implicitWidth: panelWidth
    implicitHeight: parent ? parent.height : Kirigami.Units.gridUnit * 20

    function refresh() {
        activeIndex = DocumentOutline.activeIndexForLine(outlineItems, cursorLine)
        if (activeIndex >= 0 && activeIndex < outlineList.count) {
            outlineList.currentIndex = activeIndex
            outlineList.positionViewAtIndex(activeIndex, ListView.Center)
        }
    }

    function focusList() {
        if (outlineList.count > 0) {
            outlineList.forceActiveFocus()
            refresh()
        } else {
            root.forceActiveFocus()
        }
    }

    onDocumentTextChanged: refresh()
    onCursorLineChanged: refresh()

    Rectangle {
        anchors.fill: parent
        color: Qt.tint(Kirigami.Theme.backgroundColor,
                       Qt.rgba(Kirigami.Theme.textColor.r,
                               Kirigami.Theme.textColor.g,
                               Kirigami.Theme.textColor.b,
                               0.03))
    }

    Rectangle {
        anchors.right: parent.right
        width: 1
        height: parent.height
        color: Qt.rgba(Kirigami.Theme.textColor.r,
                       Kirigami.Theme.textColor.g,
                       Kirigami.Theme.textColor.b,
                       0.08)
    }

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: Kirigami.Units.smallSpacing
        spacing: Kirigami.Units.smallSpacing

        RowLayout {
            Layout.fillWidth: true
            spacing: Kirigami.Units.smallSpacing

            Controls.Label {
                Layout.fillWidth: true
                text: i18nc("@title", "Outline")
                font.weight: Font.DemiBold
                opacity: 0.85
            }

            Controls.ToolButton {
                icon.name: "window-close-symbolic"
                display: Controls.AbstractButton.IconOnly
                flat: true
                opacity: hovered ? 1.0 : 0.65
                Layout.preferredWidth: Kirigami.Units.gridUnit * 2
                Layout.preferredHeight: Kirigami.Units.gridUnit * 2
                Controls.ToolTip.visible: hovered
                Controls.ToolTip.text: i18nc("@action", "Close outline")
                onClicked: root.closeRequested()
            }
        }

        Controls.Label {
            Layout.fillWidth: true
            visible: outlineList.count === 0
            wrapMode: Text.WordWrap
            opacity: 0.55
            text: i18nc("@info", "Add markdown headings (# Title) to navigate sections here.")
        }

        ListView {
            id: outlineList

            Layout.fillWidth: true
            Layout.fillHeight: true
            clip: true
            spacing: 2
            boundsBehavior: Flickable.StopAtBounds
            keyNavigationEnabled: true
            model: root.outlineItems

            delegate: Controls.ItemDelegate {
                id: delegateItem

                required property int index
                required property int level
                required property string title
                required property int line

                width: outlineList.width
                height: Math.max(implicitHeight, Kirigami.Units.gridUnit * 2.2)
                leftPadding: Kirigami.Units.smallSpacing + (level - 1) * Kirigami.Units.gridUnit * 0.65
                rightPadding: Kirigami.Units.smallSpacing
                topPadding: Kirigami.Units.smallSpacing * 0.35
                bottomPadding: Kirigami.Units.smallSpacing * 0.35

                highlighted: index === root.activeIndex
                hoverEnabled: true

                background: Rectangle {
                    radius: Kirigami.Units.cornerRadius * 0.5
                    color: delegateItem.highlighted
                        ? Qt.rgba(Kirigami.Theme.highlightColor.r,
                                  Kirigami.Theme.highlightColor.g,
                                  Kirigami.Theme.highlightColor.b,
                                  0.18)
                        : (delegateItem.hovered
                            ? Qt.rgba(Kirigami.Theme.textColor.r,
                                      Kirigami.Theme.textColor.g,
                                      Kirigami.Theme.textColor.b,
                                      0.06)
                            : "transparent")
                }

                contentItem: RowLayout {
                    spacing: Kirigami.Units.smallSpacing * 0.5

                    Controls.Label {
                        text: "#".repeat(level)
                        opacity: 0.42
                        font.family: Kirigami.Theme.fixedFont.family
                        font.pixelSize: Kirigami.Theme.smallerFont.pixelSize
                        Layout.alignment: Qt.AlignTop
                    }

                    Controls.Label {
                        Layout.fillWidth: true
                        text: title.length > 0
                            ? title
                            : i18nc("@label", "Untitled heading")
                        elide: Text.ElideRight
                        wrapMode: Text.NoWrap
                        font.italic: title.length === 0
                        opacity: delegateItem.highlighted ? 1.0 : 0.88
                    }
                }

                onClicked: root.navigateToLine(line)

                Keys.onReturnPressed: clicked()
                Keys.onEnterPressed: clicked()
            }

            Keys.onEscapePressed: root.closeRequested()
        }
    }
}
