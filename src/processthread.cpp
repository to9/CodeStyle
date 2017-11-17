#include "ProcessThread.h"
#include <QProcess>

ProcessThread::ProcessThread(QObject *parent)
    : QThread(parent)
{
}

ProcessThread::~ProcessThread()
{
    wait();
}

void ProcessThread::stopProcess()
{
//    mutex.lock();
//    m_cannel = false;
//    mutex.unlock();
}

void ProcessThread::processFormatCode(QStringList &filelist, QString parm)
{
    m_stringlist = filelist;
    m_parmenter = parm;
    start();
}

void ProcessThread::run()
{
    for(int i = 0; i < m_stringlist.size(); ++i)
    {
        QString execFolder("AStyle.exe ");
        execFolder.append(m_parmenter).append(" ");
        //文件路径使用双引号"",解决控制台无法识别带空格的路径问题;
        execFolder.append(QString("%1%2%3").arg("\"").arg(m_stringlist.at(i)).arg("\""));

        QProcess process;
        process.start(execFolder);
        process.waitForStarted();
        process.waitForFinished();
        QString file = QString::fromLocal8Bit(process.readAllStandardOutput());

        if(file.isEmpty())
            continue;

        emit processMessage(file, i);
        //qDebug()<<file;
        //ui->textEdit->insertPlainText(file);
    }
}


