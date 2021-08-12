import QtQuick 2.4

Item {
    id: item1
    property alias titleStr: titleTextLable.text

    Rectangle {
        id: rectangle
        height: 30
        color: "#039be5"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: -1
        anchors.leftMargin: 0
        anchors.topMargin: 0

        Text {
            id: titleTextLable
            text: "Filler"
            anchors.verticalCenter: parent.verticalCenter
            anchors.top: parent.top
            font.pixelSize: 24
            anchors.horizontalCenter: parent.horizontalCenter
            font.bold: true
            color: "white"
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

