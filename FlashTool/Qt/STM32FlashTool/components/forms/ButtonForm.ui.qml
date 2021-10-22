import QtQuick 2.0

Item {
    property alias label: textLabel.text
    property alias mouse: mouseArea
    property alias buttonColor: button.color

    height: 40

    Rectangle {
        id:button
        anchors.fill: parent
        radius: 10
        color: "#039be5"

        Text {
            id: textLabel
            anchors.fill: parent
            text: "Fill"
            color: "white"
            font.bold: true
            font.pixelSize: 27
            horizontalAlignment: Text.AlignHCenter
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent
        }
    }

}
