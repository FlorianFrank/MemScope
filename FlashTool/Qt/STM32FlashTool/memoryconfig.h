#ifndef MEMORYCONFIG_H
#define MEMORYCONFIG_H

#include <QObject>
#include <QVector>
#include <thread>
#include <QProcess>

class Compilation;

class MemoryConfig : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList memoryTypes READ getMemoryTypes WRITE setMemoryTypes  NOTIFY memoryTypesChanged)
    Q_PROPERTY(int selectedIdx READ getIndex NOTIFY dropDownIndexChanged)
    Q_PROPERTY(QString compileText READ compileText WRITE setCompileText NOTIFY compileTextChanged);
    Q_PROPERTY(QString compileStatus READ compileStatus NOTIFY compileStatusChanged);
    Q_PROPERTY(float percentage READ percentage NOTIFY percentageChanged);

public:
    explicit MemoryConfig(QObject *parent = nullptr);
    QStringList getMemoryTypes();
    int getIndex();
    void startCompilation(QString memory);
    QString& compileText();
    QString& compileStatus();
    float percentage();
    QString searchForCompileProgressInPercent(QString &message);
    QString removeBracesandPercentageSymbolFromRegexMatch(QString &regexMatch);
    int getProgressInPercentFromComilationString(QString &compileString);

signals:
    void memoryTypesChanged();
    void dropDownIndexChanged(int i);
    void compileTextChanged();
    void compileStatusChanged();
    void percentageChanged();



public slots:
    void setMemoryTypes(QStringList&);
    void compile();
    void actualElement(int);
    void readSubProcess();
    void setCompileText(QString &compileText);
    void readError();
    void startFlashing();

private:
   QStringList m_MemoryTypesList;
   int m_CurrentIdx;
   Compilation *m_Compilation;
   std::thread *m_CompileThread;
   QProcess *m_Process;
   std::vector<QString> m_CompileText;
   std::thread *th;
   QString m_CompileStatus;
   float m_Percentage;


};

#endif // MEMORYCONFIG_H

