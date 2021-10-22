import QtQuick 2.0
import QtQuick.Controls 2.0

Item {
    property alias comboBoxTitle: comboboxStr.text
    property alias comboBoxItems: chipSelectCombobox.model
    property alias combobox: chipSelectCombobox

    Column {
        id: column
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.topMargin: 0
        anchors.rightMargin: 0
        anchors.leftMargin: 0
        spacing: 2


        Text {
            id: comboboxStr
            text: "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:15pt;\">Hallo</span></p></body></html>"
            anchors.verticalCenter: parent.verticalCenter
            color: "black"
            anchors.left: parent.left
            anchors.top: parent.top
            font.pixelSize: 15
            textFormat: Text.RichText
            anchors.leftMargin: 0
            anchors.topMargin: 0
        }

        ComboBox {
            id: chipSelectCombobox
            height: 30
            anchors.left: comboboxStr.right
            anchors.right: parent.right
            anchors.rightMargin: 5
            anchors.leftMargin: 18
        }
    }
}
