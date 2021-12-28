//
// Created by florianfrank on 2021-12-26.
//

#ifndef STM32FLASHTOOL_COMPILATIONHANDLER_H
#define STM32FLASHTOOL_COMPILATIONHANDLER_H

#include <QString>
#include <QProcess>

class StartPageHandler;

class CompilationHandler: public QObject
{
Q_OBJECT
public:
    explicit CompilationHandler(StartPageHandler *memoryConfig);
    CompilationHandler() = default;

    void StartCompilation(QString memory);
    float GetPercentage() const;

    QString& GetCompileStatus();
    void SetCompileText(QString &compileText);

    std::vector<QString> &GetCompileText();

public slots:
    void readSubProcess();
    void readError();

private:

    QProcess *m_Process;
    QString m_CompileStatus;
    float m_Percentage;
    std::vector<QString> m_CompileText;
    StartPageHandler *m_MemoryConfig;
};


#endif //STM32FLASHTOOL_COMPILATIONHANDLER_H
