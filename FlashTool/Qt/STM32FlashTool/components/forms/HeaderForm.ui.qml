import QtQuick 2.4
import QtQuick.Layouts 1.0
import '.'
import '..'

Item {
    property alias titleStr: titleTextLable.text
    property alias start_page_button: start_page_button.mouse_area
    property alias test_execution_button: test_execution_button.mouse_area

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

    Menu {
        children: [
               MenuItem {
                id: start_page_button
                Layout.fillHeight: true
                Layout.fillWidth: true
                text: "Compilation"

               },

               MenuItem {
                id: test_execution_button
                Layout.fillHeight: true
                Layout.fillWidth: true
                text: "Test Execution"
               }]
        anchors.top: rectangle.bottom
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        anchors.left: parent.left
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

