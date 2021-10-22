import QtQuick 2.0
import MemoryConfig 1.0
import QtQuick.Layouts 1.0

import '.'
import '..'


Item {
    property alias flashButton: flash_button
    property alias compileButton: compile_button
    property alias  memoryDropDown: memory_selector.combobox
    property alias compileState: compile_state
    property alias compileLoggingArea: logging_area



    MemoryConfig {
        id: memoryConfig
    }


    ColumnLayout
    {
        anchors.fill: parent
        anchors.margins: 20

        SelectChipDropDown {
            id: memory_selector;
            height: 30;
            comboBoxTitle: "Select memory chip";
            comboBoxItems: memoryConfig.memoryTypes;
            Layout.fillWidth: true
            }

        Button {
            id: compile_button;
            label: "Start Compilation";
            Layout.fillWidth: true
            anchors.margins: 50

        }

        LoggingOutput {
            id: logging_area;
            height: 200;
            Layout.fillWidth: true
        }

        CompilationState {
            id:compile_state;
            compileStateLabel: memoryConfig.compileStatus;
            Layout.fillWidth: true
            anchors.margins: 50
        }

        Button {
            id: flash_button;
            label: "Flash STM board";
            Layout.fillWidth: true;

        }
    }
}
