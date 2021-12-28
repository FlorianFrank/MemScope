import './forms'

LoggingOutputForm {
textArea.onTextChanged: () => {
        textArea.cursorPosition = textArea.length-1
    }
}
