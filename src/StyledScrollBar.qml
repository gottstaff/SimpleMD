import QtQuick
import QtQuick.Controls as Controls
import org.kde.kirigami as Kirigami

Controls.ScrollBar {
    id: control

    readonly property bool prominent: control.hovered || control.pressed || control.active
    readonly property int barThickness: 3

    readonly property color thumbBase: Kirigami.Theme.textColor

    policy: Controls.ScrollBar.AsNeeded
    hoverEnabled: true
    interactive: true

    implicitWidth: control.orientation === Qt.Vertical
        ? barThickness + leftPadding + rightPadding
        : control.parent ? control.parent.width : barThickness
    implicitHeight: control.orientation === Qt.Horizontal
        ? barThickness + topPadding + bottomPadding
        : control.parent ? control.parent.height : barThickness

    leftPadding: control.orientation === Qt.Vertical ? 2 : 0
    rightPadding: control.orientation === Qt.Vertical ? 2 : 0
    topPadding: control.orientation === Qt.Horizontal ? 2 : 0
    bottomPadding: control.orientation === Qt.Horizontal ? 2 : 0

    opacity: prominent ? 1.0 : 0.55

    Behavior on opacity {
        NumberAnimation {
            duration: Kirigami.Units.longDuration
            easing.type: Easing.OutCubic
        }
    }

    contentItem: Rectangle {
        implicitWidth: control.orientation === Qt.Vertical ? control.barThickness : parent.width
        implicitHeight: control.orientation === Qt.Horizontal ? control.barThickness : parent.height
        radius: Math.min(width, height) / 2
        color: Qt.rgba(
            control.thumbBase.r,
            control.thumbBase.g,
            control.thumbBase.b,
            control.pressed ? 0.72
                : (control.prominent ? 0.58 : 0.36)
        )

        Behavior on color {
            ColorAnimation {
                duration: Kirigami.Units.longDuration
                easing.type: Easing.OutCubic
            }
        }
    }

    background: Item {
        visible: false
    }
}
