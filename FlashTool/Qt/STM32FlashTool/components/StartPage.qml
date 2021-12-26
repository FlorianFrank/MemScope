import './forms'

StartPageForm {



    frequency_slider.onValueChanged: () => {
                                         frequency_percentage_label.text = frequency_slider.value + " MHz"
                                         memoryConfig.setFrequency(frequency_slider.value);
                                     }


    check_box_UART4_input.onCheckStateChanged:    () => { memoryConfig.setIOInterfaceEnable(qsTr("Input"), qsTr("UART4"), check_box_UART4_input.checked) }
    check_box_UART5_input.onCheckStateChanged:    () => { memoryConfig.setIOInterfaceEnable(qsTr("Input"), qsTr("UART5"), check_box_UART5_input.checked) }
    check_box_SPI1_input.onCheckStateChanged:     () => { memoryConfig.setIOInterfaceEnable(qsTr("Input"), qsTr("SPI1"), check_box_SPI1_input.checked) }
    check_box_SPI2_input.onCheckStateChanged:     () => { memoryConfig.setIOInterfaceEnable(qsTr("Input"), qsTr("SPI2"), check_box_SPI2_input.checked) }
    check_box_USB_HOST_input.onCheckStateChanged: () => { memoryConfig.setIOInterfaceEnable(qsTr("Input"), qsTr("USB_HOST"), check_box_USB_HOST_input.checked) }

    check_box_UART4_output.onCheckStateChanged:    () => { memoryConfig.setIOInterfaceEnable(qsTr("Output"), qsTr("UART4"), check_box_UART4_output.checked) }
    check_box_UART5_output.onCheckStateChanged:    () => { memoryConfig.setIOInterfaceEnable(qsTr("Output"), qsTr("UART5"), check_box_UART5_output.checked) }
    check_box_SPI1_output.onCheckStateChanged:     () => { memoryConfig.setIOInterfaceEnable(qsTr("Output"), qsTr("SPI1"), check_box_SPI1_output.checked) }
    check_box_SPI2_output.onCheckStateChanged:     () => { memoryConfig.setIOInterfaceEnable(qsTr("Output"), qsTr("SPI2"), check_box_SPI2_output.checked) }
    check_box_USB_HOST_output.onCheckStateChanged: () => { memoryConfig.setIOInterfaceEnable(qsTr("Output"), qsTr("USB_HOST"), check_box_USB_HOST_output.checked) }
}
