import QtQuick 2.4
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0

Item {
    GridLayout {
        id: grid_layout
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        anchors.margins: 5
        flow: GridLayout.LeftToRight
        columns: 2
        rows: 10

        Rectangle {
            id: rectangle
            color: "#039be5"
            radius: 10
            Layout.fillHeight: true
            Layout.fillWidth: true

            Text {
                id: memory_selection_label
                text: 'Select test: '
                color: "white"
                font.pixelSize: 27
                anchors.centerIn: parent
            }
        }

        Rectangle {
            id: rectangle2
            Layout.fillHeight: true
            Layout.fillWidth: true


            ComboBox {
                id: comboBox
                anchors.fill: parent
                currentIndex: 2
                anchors.margins: 5
                model: ["Test alternating", "Test periodic counter", "Test write zeros", "Test write ones"]
            }
        }


        Rectangle {
            id: test_state
            color: "#039be5"
            radius: 10
            Layout.fillHeight: true
            Layout.fillWidth: true

            Text {
                id: test_state_label
                color: "white"
                font.pixelSize: 27
                text: qsTr("Test status:")
                anchors.centerIn: parent
            }
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true

            Text {
                id: test_execution_state
                text: qsTr("Passed")
                color: "white"
                font.pixelSize: 27
                anchors.centerIn: parent
            }
        }

        Rectangle {
            Layout.fillHeight: true
            Layout.fillWidth: true
            color: "#039be5"
            radius: 10
            Layout.columnSpan: 2

            Text {
                id: log_label
                color: "white"
                font.pixelSize: 27
                text: qsTr("Logging")
                anchors.centerIn: parent
            }
        }

        Rectangle {
            Layout.columnSpan: 2
            Layout.rowSpan: 7
            height: 9
            Layout.fillHeight: true
            Layout.fillWidth: true
            border.color: 'black'
            border.width: 1
            TextArea {
                anchors.fill: parent
                id: logging_area
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;formeditorZoom:0.75;height:480;width:640}
}
##^##*/

