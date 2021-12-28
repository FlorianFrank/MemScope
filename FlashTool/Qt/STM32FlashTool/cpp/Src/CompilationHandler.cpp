//
// Created by florianfrank on 2021-12-26.
//

#include "CompilationHandler.h"
#include "StartPageHandler.h"

#include <QDir>
#include <QDebug>
#include <ConsoleOutputParser.h>
#include <QTextCodec>
#include <FlashHandler.h>
#include "EasyLoggingWrapper.h"


#define CMAKE_TIMEOUT 30000


CompilationHandler::CompilationHandler(StartPageHandler *memoryConfig) : QObject(nullptr),
                                                                     m_MemoryConfig(memoryConfig),
                                                                     m_Percentage(0)
{
    m_CompileStatus = "Waiting for compilation\n";
    LOG(INFO) << "Initialize CompilationHandler";
}


void CompilationHandler::StartCompilation(QString memory)
{
    QDir tmpDir("tmp");
    if (tmpDir.exists())
    {
        LOG(INFO) << "Remove tmp folder";
        tmpDir.removeRecursively();
    }
    QDir().mkdir("tmp");
    LOG(INFO) << "tmp dir created";

    m_Process = new QProcess;

    connect(m_Process, SIGNAL(readyReadStandardOutput()), this, SLOT(readSubProcess()));
    connect(m_Process, SIGNAL(readyReadStandardError()), this, SLOT(readError()));

    QString command =
            "bash /home/florianfrank/Documents/Research/Projects/PUFMem/stm_measurement_firmware/FlashTool/Qt/STM32FlashTool/compile_fw_STM32F4.sh " +
            memory + " 2>&1";
    m_Process->setWorkingDirectory("./tmp");
    m_Process->setProgram("cmake");
    QString absBuildPath;
    tmpDir.absoluteFilePath(absBuildPath);
    QStringList arguments = {"../../../../..", "-DBuildFolder=" + absBuildPath, "-DBoardName='stm32f429'",
                             "-DMEMORY_TYPE=" + memory + "=1", "-DBoardClass=STM32F4", "-DCPU=cortex-m4",
                             "-DFPUType=hard", "-DFPUSpecification=fpv4-sp-d16", "-Dspecs=rdimon.specs",
                             "-DOS_USE_SEMIHOSTING=1"};
    m_Process->setArguments(arguments);

    auto stringStreamToOutput = [&arguments]() {
        std::stringstream stringstream;
        for(const auto& args: arguments)
            stringstream << args.toStdString();
        return stringstream.str();
    };

    LOG(INFO) << "Run CMAKE with arguments: " << stringStreamToOutput();

    m_Process->start();
    bool finished = m_Process->waitForFinished(30000);

    if(!finished)
    {
        LOG(ERROR) << "Error Process Cmake could not be finished within " << CMAKE_TIMEOUT << " milliseconds";
        return; // TODO exception
    }

    LOG(INFO) << "CMake finished";

    m_Process->reset();
    m_Process->setArguments(QStringList("-d"));
    m_Process->setProgram("make");

    m_Process->setWorkingDirectory("./tmp");
    LOG(INFO) << "Run make";
    m_Process->start();

    m_CompileStatus = "compiling";
    emit m_MemoryConfig->compileStatusChanged();
//});

}

void CompilationHandler::readSubProcess()
{
    QByteArray ret = m_Process->readAllStandardOutput();
    LOG(INFO) << "Get message from compile process on std out: " << ret.toStdString();

    QString compileText = QTextCodec::codecForMib(106)->toUnicode(ret);
    int percentage = ConsoleOutputParser::getProgressInPercentFromComilationString(compileText);
    if (percentage != -1)
    {
        m_Percentage = ((float) percentage) / 100;
        emit m_MemoryConfig->percentageChanged();

    }
    emit SetCompileText(compileText);
}

void CompilationHandler::readError()
{
    QString compileText = m_Process->readAllStandardError();
    LOG(ERROR) << "Get message from compile process on std err: " << compileText.toStdString();

    //qDebug() << compileText;
    //m_CompileStatus = "error";
    //emit compileStatusChanged();
    emit SetCompileText(compileText);
}

void CompilationHandler::SetCompileText(QString &compileText)
{
    m_CompileText.push_back(compileText);

    QString match = "Successfully remade target file 'default_target'";

    if(compileText.contains(match))
    {
        LOG(INFO) << "Successfully remade target file 'default_target found in return string";
        QStringRef subString(&compileText, compileText.indexOf("Successfully remade target file"), compileText.indexOf("Successfully remade target file") +20); // subString contains "is"
        m_CompileStatus = "success";
        emit m_MemoryConfig->compileStatusChanged();
    }

    LOG(INFO) << "Set compile text changed";
    emit m_MemoryConfig->compileTextChanged();
}

float CompilationHandler::GetPercentage() const
{
    return m_Percentage;
}

QString &CompilationHandler::GetCompileStatus()
{
    return m_CompileStatus;
}

std::vector<QString> &CompilationHandler::GetCompileText()
{
    return m_CompileText;
}
