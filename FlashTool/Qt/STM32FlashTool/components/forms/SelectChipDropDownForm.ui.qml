import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Item {
    property alias comboBoxTitle: comboboxStr.text
    property alias comboBoxItems: chipSelectCombobox.model
    property alias combobox: chipSelectCombobox

    RowLayout {
        id: row
        anchors.fill: parent
        anchors.margins: 2

        Text {
            id: comboboxStr
            text: "Loram ipsum"
            Layout.fillWidth: true
            color: "black"
            width: parent.width * 0.3
            Layout.alignment: Qt.AlignLeft
        }

        ComboBox {
            id: chipSelectCombobox
            clip: false
            Layout.fillWidth: true
            anchors.rightMargin: 0
            Layout.alignment: Qt.AlignRight
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;formeditorZoom:0.66;height:480;width:640}
}
##^##*/

