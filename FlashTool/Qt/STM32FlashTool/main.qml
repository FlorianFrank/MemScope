import QtQuick 2.12
import QtQuick.Window 2.12
import MemoryConfig 1.0

Window {
    id: window
    width: 640
    height: 480
    visible: true

    MemoryConfig {
        id: memoryConfig
    }

    HeaderForm { id: header; anchors.left: parent.left; anchors.right: parent.right; anchors.top: parent.top; anchors.rightMargin: 0; anchors.leftMargin: 0; anchors.topMargin: 0; titleStr: "MemoryTest" }
    SelectChipDropDown{ id: memorySelector; comboBoxTitle: "Select memory chip"; combobox.onActivated: memoryConfig.actualElement(combobox.currentIndex); anchors.left: parent.left; anchors.right: parent.right; anchors.top: header.bottom; anchors.rightMargin: 5; anchors.leftMargin: 5; anchors.topMargin: 39; comboBoxItems: memoryConfig.memoryTypes}

    Button { id: button; mouse.onClicked: memoryConfig.compile(); anchors.left: memorySelector.left; anchors.right: memorySelector.right; anchors.top: memorySelector.bottom; anchors.bottom: memorySelector.top; anchors.rightMargin: 40; anchors.leftMargin: 40; anchors.bottomMargin: -73; anchors.topMargin: 35; label: "Start Compilation"}

    LoggingOutput { textAreaStr: memoryConfig.compileText; anchors.left: parent.left; anchors.right: parent.right; anchors.top: button.bottom; anchors.bottom: parent.bottom; anchors.rightMargin: 40; anchors.leftMargin: 40; anchors.bottomMargin: 60; anchors.topMargin: 11;}
}

/*##^##
Designer {
    D{i:0;formeditorZoom:1.5}
}
##^##*/
