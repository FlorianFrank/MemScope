import QtQuick 2.4
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0

Item {
    GridLayout {
        anchors.margins: 10
        anchors.fill: parent
        columns: 2

        Rectangle {
            Layout.fillHeight: true
            width: 100

            Layout.columnSpan: 2
            color: "grey"

            Label {
                anchors.centerIn: parent
                text: "Select Tests"
                color: "white"
                font.pixelSize: 20
            }
        }

        Rectangle {
            Layout.fillHeight: true
            width: 200

            color: "blue"

            Label {
                text: "Select Test:"
                color: "white"
                font.pixelSize: 20
            }
        }

        ComboBox {
            Layout.fillHeight: true
            width: 200
            model: ["Test Simple Read and write operation"]
        }

        GridLayout {
            Layout.columnSpan: 2

            Rectangle {
                width: 100
                Layout.fillHeight: true
                color: "blue"

                Label {
                    text: "Init:"
                    color: "white"
                    font.pixelSize: 20
                }
            }

            TextArea {
                Layout.fillHeight: true
                Layout.fillWidth: true
                text: "Memory mem = InitializeMemory();"
                Layout.rowSpan: 2
            }
        }

        GridLayout {
            Layout.columnSpan: 2

            Rectangle {
                width: 100
                Layout.fillHeight: true
                color: "blue"

                Label {
                    text: "Running:"
                    color: "white"
                    font.pixelSize: 20
                }
            }

            TextArea {
                Layout.fillHeight: true
                Layout.fillWidth: true
                text: "ReadMemory(mem1);\n Delay(10ms); ReadMemory(mem1)"
                Layout.rowSpan: 2
            }
        }

        GridLayout {
            Layout.columnSpan: 2

            Rectangle {
                width: 100
                Layout.fillHeight: true
                color: "blue"

                Label {
                    text: "Done:"
                    color: "white"
                    font.pixelSize: 20
                }
            }

            TextArea {
                Layout.fillHeight: true
                Layout.fillWidth: true
                text: "DeinitializeMemory(mem1);"

                Layout.rowSpan: 2
            }
        }
    }
}
/*##^##
Designer {
    D{i:0;autoSize:true;formeditorZoom:0.75;height:480;width:640}
}
##^##*/

