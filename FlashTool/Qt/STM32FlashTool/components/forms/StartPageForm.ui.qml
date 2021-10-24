import QtQuick 2.0
import MemoryConfig 1.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.5
import QtQuick.Controls.Styles 1.0

import "."
import ".."

Item {
    property alias flashButton: flash_button
    property alias compileButton: compile_button
    property alias memoryDropDown: memory_selector.combobox
    property alias compileState: compile_state
    property alias compileLoggingArea: logging_area
    property alias compileProgress: compile_state_progres_bar.value

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20

        SelectChipDropDown {
            id: memory_selector
            height: 30
            Layout.bottomMargin: 5
            comboBoxTitle: "Select memory chip"
            comboBoxItems: memoryConfig.memoryTypes
            Layout.fillWidth: true
        }

        Button {
            id: compile_button
            label: "Start Compilation"
            Layout.fillWidth: true
            anchors.margins: 50
            Layout.topMargin: 5
            Layout.margins: 0
            height: parent.height * 0.1
        }

        LoggingOutput {
            id: logging_area
            height: parent.height * 0.8
            Layout.fillWidth: true
        }

        ProgressBar {
            id: compile_state_progres_bar
            anchors.left: parent.left
            anchors.right: parent.right
            height: parent.height * 0.1
            onValueChanged: () => {
                                console.log("VALUE " + memoryConfig.percentage)
                            }
        }

        CompilationState {
            id: compile_state
            compileStateLabel: memoryConfig.compileStatus
            Layout.fillWidth: true
            anchors.margins: 50
        }

        Button {
            id: flash_button
            label: "Flash STM board"
            Layout.fillWidth: true
            height: parent.height * 0.1
        }
    }
}

/*##^##
Designer {
    D{i:0;autoSize:true;height:480;width:640}
}
##^##*/

