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
public:
    explicit MemoryConfig(QObject *parent = nullptr);
    QStringList getMemoryTypes();
    int getIndex();
    void startCompilation(QString memory);
    QString& compileText();

signals:
    void memoryTypesChanged();
    void dropDownIndexChanged(int i);
    void compileTextChanged();



public slots:
    void setMemoryTypes(QStringList&);
    void compile();
    void actualElement(int);
    void readSubProcess();
    void setCompileText(QString &compileText);
    void readError();






private:
   QStringList m_MemoryTypesList;
   int m_CurrentIdx;
   Compilation *m_Compilation;

   std::thread *m_CompileThread;
   QProcess *m_Process;
   QString m_CompileText;
   std::thread *th;

};

#endif // MEMORYCONFIG_H

