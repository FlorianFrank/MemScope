import QtQuick 2.0

Item {
    property alias label: textLabel.text
    property alias mouse: mouseArea
    property alias buttonColor: button.color

    Rectangle {
        id: button
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

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
