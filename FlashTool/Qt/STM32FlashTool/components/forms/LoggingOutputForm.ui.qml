import QtQuick 2.0
import QtQuick.Controls 2.12

Item {

    property alias textAreaStr: loggingDisplay.text
    property alias textArea: loggingDisplay

    Text {
        id: loggingText
        text: "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\np, li { white-space: pre-wrap; }\n</style></head><body style=\" font-family:'Ubuntu'; font-size:11pt; font-weight:400; font-style:normal;\">\n<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:17pt;\">Output:</span></p></body></html>"
        textFormat: Text.RichText
    }

    Rectangle {
        id: loggingFrame
        color: "#fafafa"
        anchors.topMargin: 7
        border.color: "#039be5"
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: loggingText.bottom
        anchors.bottom: parent.bottom
        radius: 20

        ScrollView {
            id: view
            anchors.fill: parent

            TextArea {
                id: loggingDisplay
                textFormat: Text.RichText
                anchors.fill: parent
                wrapMode: Text.WordWrap
                anchors.topMargin: 0
            }
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;formeditorZoom:2;height:480;width:640}
}
##^##*/

