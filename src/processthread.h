#ifndef PROCESSTHREAD_H
#define PROCESSTHREAD_H

#include <QThread>
#include <QMutex>
#include <QStringList>

class ProcessThread : public QThread
{
    Q_OBJECT

public:
    ProcessThread(QObject *parent = 0);
    ~ProcessThread();

public:
    void processFormatCode(QStringList &filelist, QString parm);

signals:
    void processMessage(QString, int);

public slots:
    void stopProcess();

protected:
    void run();

private:
    QMutex mutex;
    QString m_parmenter;
    QStringList m_stringlist;
};

#endif // PROCESSTHREAD_H
