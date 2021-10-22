import './forms'

CompilationStateForm {

    function getCompileFlagColor()
    {
        if(memoryConfig.compileStatus === "success")
            return "green"
        if(memoryConfig.compileStatus === "compiling")
            return "blue"
        if(memoryConfig.compileStatus === "error")
            return "red"
        if(memoryConfig.compileStatus === "warning")
            return "yellow"
        return "lightGrey"
    }
}

