import QtQuick 2.4

Item {

    property alias mouse_area: mouse_area
    property alias text: item_text.text
    property alias item_frame: item_frame

    Rectangle {
        id: item_frame
        color: '#8d8d8d'
        radius: 5
        anchors.fill: parent
        anchors.margins: 1

        Text {
            id: item_text
            text: "Item"
            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            minimumPointSize: 50
            anchors.centerIn: parent.Center
            color: "white"
        }
    }

    MouseArea {
        id: mouse_area
        anchors.fill: parent
        hoverEnabled: true
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/
