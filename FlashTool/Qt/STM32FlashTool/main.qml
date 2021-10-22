import QtQuick 2.12
import QtQuick.Window 2.12
import MemoryConfig 1.0

import './components/'



Window {
    id: window
    width: 700
    height: 480
    visible: true

    function set_current_view(idx) {
        if(idx === 0)
        {
            startPage.visible = true
            testWindow.visible = false
        }
        if(idx === 1)
        {
            startPage.visible = false
            testWindow.visible = true
        }
    }


    // Contains the memories used in the ListView component
    MemoryConfig {
        id: memoryConfig
    }


     Header {
         id: header;
         start_page_button.onClicked: set_current_view(0)
         test_execution_button.onClicked: set_current_view(1)
         anchors.left: parent.left;
         anchors.right: parent.right;
         anchors.bottom: parent.bottom
         anchors.bottomMargin: 419
         anchors.top: parent.top;
         anchors.rightMargin: 0;
         anchors.leftMargin: 0;
         anchors.topMargin: 0;
         titleStr: "Memory Test"
     }


    StartPage {
        id: startPage
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
        anchors.rightMargin: 0
        anchors.bottomMargin: 0
        anchors.leftMargin: 0
        anchors.topMargin: 0
        anchors.left: parent.left
        anchors.right: parent.right
        flashButton.mouse.onClicked: {
                testWindow.visible = true
                startPage.visible = false
        }

        flashButton.buttonColor:flashButtonActive()
        compileButton.mouse.onClicked: memoryConfig.compile()
        memoryDropDown.onActivated: memoryConfig.actualElement(memoryDropDown.currentIndex)
        compileState.compileFlagColor: getCompileFlagColor();
        compileLoggingArea.textAreaStr: memoryConfig.compileText;
    }

    TestWindow {
        id: testWindow
        visible: false
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
        anchors.rightMargin: 0
        anchors.bottomMargin: 0
        anchors.leftMargin: 0
        anchors.topMargin: 0
        anchors.left: parent.left
        anchors.right: parent.right
    }


    function flashButtonActive() {

    if(memoryConfig.compileStatus === "success") {
        return "green"
    }
    else
    return "grey"
}



}


/*##^##
Designer {
    D{i:0;formeditorZoom:0.75}
}
##^##*/
