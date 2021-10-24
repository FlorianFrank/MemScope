import QtQuick 2.4
import QtQuick.Layouts 1.0
import "."

Item {

    property alias children: row_layout.children

    Column {
        anchors.fill: parent
        Rectangle {
            id: rectangle
            height: parent.height
            color: 'grey'
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.top: parent.top

            RowLayout {
                id: row_layout
                anchors.fill: parent
                anchors.margins: 2
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

