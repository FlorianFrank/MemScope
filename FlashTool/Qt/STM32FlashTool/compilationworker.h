#ifndef COMPILATIONWORKER_H
#define COMPILATIONWORKER_H

#include <QObject>

class CompilationWorker : public QObject
{
    Q_OBJECT
public:
    explicit CompilationWorker(QObject *parent = nullptr);
private:
    QObject* m_obj;
signals:
    void deleteObject(QObject* thingy);
private slots:
    void compile(QString &compile);

};

#endif // COMPILATIONWORKER_H
