import QtQuick 2.0

Item {
    width: 60
    height: 30

    property alias compileStateLabel: statusLabel.text;
    property alias compileLabel: statusLabel
    property alias compileFlagColor: compileFlag.color

    Rectangle {
        id: compilationStateFrame
        radius: 2
        color: "#fafafa"
        border.color: "#039be5"
        anchors.fill: parent

            Text {
                id: statusLabel
                width: 15
                text: "status"
                anchors.left: parent.left
                anchors.leftMargin: 3
            }

            Rectangle {
                id: compileFlag
                width: 20
                height: 20
                radius: width / 2
                border.width: 1
                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right

                anchors.topMargin: 0
                anchors.bottomMargin: 0
                anchors.rightMargin: 7
            }
    }
}