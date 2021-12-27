import QtQuick 2.0
import StartPageHandler 1.0
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
    property alias frequency_slider: frequency_slider
    property alias frequency_percentage_label: frequency_percentage_label

    property alias check_box_UART4_input: checkBoxUART4Input
    property alias check_box_UART5_input: checkBoxUART5Input
    property alias check_box_SPI1_input: checkBoxSPI1Input
    property alias check_box_SPI2_input: checkBoxSPI2Input
    property alias check_box_USB_HOST_input: checkBoxUSBHostInput

    property alias check_box_UART4_output: checkBoxUART4Output
    property alias check_box_UART5_output: checkBoxUART5Output
    property alias check_box_SPI1_output: checkBoxSPI1Output
    property alias check_box_SPI2_output: checkBoxSPI2Output
    property alias check_box_USB_HOST_output: checkBoxUSBHostOutput

    width: 1000
    height: 1500
    property alias label: label

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20

        RowLayout {
            Label {
                text: "Select board:"
            }
            ComboBox {
                id: boardType
                height: 30
                Layout.bottomMargin: 20
                model: ["STM32F429 DISC1"]
                Layout.fillWidth: true
                width: parent.width * 0.3
            }
        }

        SelectChipDropDown {
            id: memory_selector
            height: 30
            Layout.alignment: Qt.AlignHCenter | Qt.AlignTop
            Layout.bottomMargin: 20
            comboBoxTitle: "Select memory chip:"
            comboBoxItems: memoryConfig.memoryTypes
            Layout.fillWidth: true
        }

        RowLayout {
            anchors.topMargin: 20
            spacing: 100
            ColumnLayout {
                Label {
                    text: "Set Input Interface:"
                    font.bold: true
                }

                CheckBox {
                    id: checkBoxUART4Input
                    checked: false
                    text: qsTr("UART4")
                }

                CheckBox {
                    id: checkBoxUART5Input
                    checked: false
                    text: qsTr("UART5")
                }

                CheckBox {
                    id: checkBoxSPI1Input
                    checked: false
                    text: qsTr("SPI1")
                }

                CheckBox {
                    id: checkBoxSPI2Input
                    checked: false
                    text: qsTr("SPI2")
                }
                CheckBox {
                    id: checkBoxUSBHostInput
                    checked: false
                    text: qsTr("USB Host")
                }
            }

            ColumnLayout {
                Label {
                    id: label
                    text: "Set output interface:"
                    font.bold: true
                }

                CheckBox {
                    id: checkBoxUART4Output
                    checked: false
                    text: qsTr("UART4")
                }

                CheckBox {
                    id: checkBoxUART5Output
                    checked: false
                    text: qsTr("UART5")
                }

                CheckBox {
                    id: checkBoxSPI1Output
                    checked: false
                    text: qsTr("SPI1")
                }

                CheckBox {
                    id: checkBoxSPI2Output
                    checked: false
                    text: qsTr("SPI2")
                }
                CheckBox {
                    id: checkBoxUSBHostOutput
                    checked: false
                    text: qsTr("USB Host")
                }
            }

            ColumnLayout {
                Label {
                    text: qsTr("Set clock frequency:")
                    font.bold: true
                }

                Slider {
                    id: frequency_slider
                    from: 100
                    to: 180
                    value: 180
                    stepSize: 1
                    snapMode: Slider.SnapAlways
                }

                Label {
                    id: frequency_percentage_label
                    text: "180 MHz"
                    horizontalAlignment: Text.AlignHCenter
                    font.pointSize: 20
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    Layout.fillHeight: false
                    Layout.fillWidth: false
                }
            }
        }

        Button {
            id: compile_button
            label: "Start Compilation"
            Layout.fillWidth: true
            anchors.margins: 50
            Layout.topMargin: 5
            Layout.margins: 0
            height: parent.height * 0.07
        }

        LoggingOutput {
            id: logging_area
            height: parent.height * 0.4
            Layout.fillWidth: true
        }

        ProgressBar {
            id: compile_state_progres_bar
            height: parent.height * 0.07
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
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
            height: parent.height * 0.07
        }
    }
}

/*##^##
Designer {
    D{i:0;formeditorZoom:0.5}
}
##^##*/
