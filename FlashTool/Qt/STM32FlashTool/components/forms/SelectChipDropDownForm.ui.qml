import QtQuick 2.0
import QtQuick.Controls 2.0
import QtQuick.Layouts 1.0

Item {
    property alias comboBoxTitle: comboboxStr.text
    property alias comboBoxItems: chipSelectCombobox.model
    property alias combobox: chipSelectCombobox

    Row {
        id: column
        anchors.fill: parent
        anchors.margins: 2

        Text {
            id: comboboxStr
            text: "Loram ipsum"
            color: "black"
            width: parent.width * 0.3
        }

        ComboBox {
            id: chipSelectCombobox
            anchors.right: parent.right
            width: parent.width * 0.6
        }
    }
}
