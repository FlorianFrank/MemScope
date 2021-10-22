import QtQuick 2.4
import QtQuick.Layouts 1.0
import '.'

Item {

    property alias children: row_layout.children

    Rectangle {
        id: rectangle
        height: 30
        color: 'grey'
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.rightMargin: -1
        anchors.leftMargin: 0
        anchors.topMargin: 0

        RowLayout {
            id: row_layout
            anchors.fill: parent

        }


    }
}

