#include "memoryconfig.h"
#include <QtDebug>
#include <stdlib.h>
#include <QTextCodec>
#include <QDir>
#include <unistd.h>

MemoryConfig::MemoryConfig(QObject *parent) : QObject(parent), m_CompileStatus("waiting for compilation"),
    m_MemoryTypesList({"Please select the memory module",
                      "SRAM_ALLIANCE_AS7C34098A",
                      "SRAM_ALLIANCE_AS7C38098A",
                      "SRAM_CYPRESS_CY62157EV30",
                      "SRAM_CYPRESS_CY7C1041CV33",
                      "SRAM_CYPRESS_CY7C1051DV33",
                      "SRAM_ISSI_IS61WV51216BLL",
                      "MRAM_EVERSPIN_MR4A08BYS35",
                      "MRAM_EVERSPIN_MR4A08BUYS45",
                      "FRAM_ROHM_MR48V256CTAZAARL",
                      "FRAM_FUJITSU_MB85R1001ANC_GE1",
                      "RERAM_ADESTO_RM25C512C_LTAI_T",
                      "RERAM_FUJITSU_MB85AS4MTPF_G_BCERE1",
                      "FRAM_CYPRESS_FM22L16_55_TG"}), m_CurrentIdx(0)
{

}


QStringList MemoryConfig::getMemoryTypes()
{
    return m_MemoryTypesList;
}

int getIndex()
{
    return 0; // TODO
}

void MemoryConfig::setMemoryTypes(QStringList& memories)
{
    if (m_MemoryTypesList != memories)
    {
        m_MemoryTypesList = memories;
        emit memoryTypesChanged();
    }
}


void MemoryConfig::actualElement(int idx) {
    m_CurrentIdx = idx;
}

void MemoryConfig::compile()
{
    if(m_CurrentIdx != 0)
    {
        startCompilation(m_MemoryTypesList.at(m_CurrentIdx));
    }
    qDebug() << "Start compilation";
}




int MemoryConfig::getIndex()
{
return 0;
}


void MemoryConfig::startFlashing()
{
    qDebug() << "Start flashing";
    m_CompileText.push_back("Start flashing ...");
    emit setCompileText(m_CompileText[m_CompileText.size() -1]);
}

/*static*/ void MemoryConfig::startCompilation(QString memory)
{

           QDir tmpDir("tmp");
           if(tmpDir.exists())
                tmpDir.removeRecursively();
           QDir().mkdir("tmp");
           qDebug() << "Create dir tmp";

           m_Process = new QProcess;


           connect(m_Process, SIGNAL(readyReadStandardOutput()),
                        this, SLOT(readSubProcess()));

           connect(m_Process, SIGNAL(readyReadStandardError()),
                        this, SLOT(readError()));


          // th = new std::thread(
           //[this, memory](){

           QString command = "bash /home/florianfrank/Documents/Research/Projects/PUFMem/stm_measurement_firmware/FlashTool/Qt/STM32FlashTool/compile_fw_STM32F4.sh " + memory + " 2>&1";
           m_Process->setWorkingDirectory("./tmp");
           m_Process->setProgram("cmake");
           QString absBuildPath;
           tmpDir.absoluteFilePath(absBuildPath);
           QStringList arguments = {"../../../..", "-DBuildFolder=" + absBuildPath, "-DBoardName='stm32f429'", "-DMEMORY_TYPE="+memory+"=1", "-DBoardClass=STM32F4", "-DCPU=cortex-m4", "-DFPUType=hard", "-DFPUSpecification=fpv4-sp-d16", "-Dspecs=rdimon.specs", "-DOS_USE_SEMIHOSTING=1"};
           m_Process->setArguments(arguments);
           m_Process->start();
           m_Process->waitForFinished(30000);

           m_Process->reset();
           m_Process->setArguments(QStringList("-d"));
           m_Process->setProgram("make");

           m_Process->setWorkingDirectory("./tmp");
           m_Process->start();

           m_CompileStatus = "compiling";
           emit compileStatusChanged();
           //});

}


void MemoryConfig::readSubProcess()
{
     QByteArray ret = m_Process->readAllStandardOutput();
    QString compileText = QTextCodec::codecForMib(106)->toUnicode(ret);

    emit setCompileText(compileText);
}

void MemoryConfig::readError()
{
    QString compileText = m_Process->readAllStandardError();
    qDebug() << compileText;
    //m_CompileStatus = "error";
    //emit compileStatusChanged();
    emit setCompileText(compileText);
}

QString& MemoryConfig::compileText()
{
    if (m_CompileText.size() == 0)
    {
        m_CompileText.push_back(""); // TODO fix me
    }
    return m_CompileText[m_CompileText.size() -1];
}


void MemoryConfig::setCompileText(QString &compileText)
{
    m_CompileText.push_back(compileText);

    QString match = "Successfully remade target file 'default_target'";

    if(compileText.contains(match))
    {
        QStringRef subString(&compileText, compileText.indexOf("Successfully remade target file"), compileText.indexOf("Successfully remade target file") +20); // subString contains "is"
        qDebug() << subString;
            m_CompileStatus = "success";
            emit compileStatusChanged();
    }

    emit compileTextChanged();
}

QString& MemoryConfig::compileStatus()
{
    return m_CompileStatus;
}
