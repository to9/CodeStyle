#include "widget.h"
#include "ui_widget.h"

#include <QDir>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QProcess>
#include <QMessageBox>
#include <QMenu>
#include <QSettings>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QSettings>

#include "processthread.h"

Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    setWindowTitle(tr("CodeStyle"));

    QString settingFile;
    settingFile = QCoreApplication::applicationDirPath();
    settingFile+="/CodeStyle.ini";
    m_settings = new QSettings(settingFile, QSettings::IniFormat, this);
    m_settings->setIniCodec("UTF8");

    QFileInfo fileInfo(settingFile);
    if(!fileInfo.isFile())
    {
        m_settings->setValue("AstyleConfig", QVariant("--suffix=none --lineend=windows --style=allman --indent=tab --indent-switches --indent-preproc-define --pad-oper --unpad-paren --align-pointer=name --add-braces --indent-col1-comments"));
    }

    m_astyleConfig = m_settings->value("AstyleConfig").toString();
    ui->lineEdit_2->setText(m_astyleConfig);

    QObject::connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(loadFile()));
    QObject::connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(formatCode()));

    m_thread = new ProcessThread(this);
    QObject::connect(m_thread, SIGNAL(processMessage(QString, int)), this, SLOT(setFormatCodeFile(QString, int)));

    standardItemModel = new QStandardItemModel(this);
    ui->listView->setModel(standardItemModel);
    ui->listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(ui->listView, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(showSelectMenu(const QPoint&)));
}

Widget::~Widget()
{
    delete ui;
}

void Widget::closeEvent(QCloseEvent *event)
{
    Q_UNUSED(event)
    m_settings->setValue("AstyleConfig", QVariant(ui->lineEdit_2->text()));
}

void Widget::showSelectMenu(const QPoint &pos)
{
    Q_UNUSED(pos)

    QMenu *menu = new QMenu(ui->listView);

    QAction *addFileAction = menu->addAction("添加文件...");
    connect(addFileAction, SIGNAL(triggered(bool)), this, SLOT(addFileActionMenuItem()));
    QAction *addFolderAction = menu->addAction("添加文件夹...");
    connect(addFolderAction, SIGNAL(triggered(bool)), this, SLOT(addFolderActionMenuItem()));
    QAction *addRecursiveFolderAction = menu->addAction("递归添加文件夹...");
    connect(addRecursiveFolderAction, SIGNAL(triggered(bool)), this, SLOT(addRecursiveFolderActionMenuItem()));

   if(!(ui->listView->selectionModel()->selectedIndexes().empty()))
   {
       QAction *deleteAction = menu->addAction("移除文件");
       connect(deleteAction, SIGNAL(triggered(bool)), this, SLOT(deleteActionMenuItem()));
       QAction *clearrAction = menu->addAction("移除所有文件");
       connect(clearrAction, SIGNAL(triggered(bool)), this, SLOT(clearActionMenuItem()));
   }
   menu->exec(QCursor::pos());
}

void Widget::deleteActionMenuItem(void)
{
    QModelIndexList selectIndexList = ui->listView->selectionModel()->selectedRows();
    while(!selectIndexList.isEmpty())
    {
        QModelIndexList indexList;
        QModelIndex index_1, newIndex;
        foreach(index_1, selectIndexList)
        {
            if(newIndex.row() != index_1.row())
            {
                newIndex = index_1;
                indexList.append(newIndex);
            }
        }

        QModelIndex index = indexList.first();
        standardItemModel->removeRow(index.row());
        selectIndexList = ui->listView->selectionModel()->selectedRows();
    }
}

bool Widget::isExistFilePath(QString &filePath)
{
    for(int i = 0; i<standardItemModel->rowCount(); i++)
    {
        QString existFilePath = standardItemModel->item(i)->text();
        if(0 == QString::compare(existFilePath, filePath, Qt::CaseInsensitive))
        {
            return true;
        }
    }
    return false;
}

void Widget::addFileActionMenuItem(void)
{
    QFileDialog *fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(tr("添加文件"));
    fileDialog->setNameFilter("*.h *.c *.cpp");
    fileDialog->setFileMode(QFileDialog::ExistingFiles);
    fileDialog->setViewMode(QFileDialog::Detail);
    QStringList fileNames;
    if(fileDialog->exec())
    {
        fileNames = fileDialog->selectedFiles();
    }

    if(!fileNames.isEmpty())
    {
        for(int i = 0; i< fileNames.count(); i++)
        {
            QString filePath = fileNames.at(i);
            filePath.replace("/","\\");//windows
            filePath.prepend("        ");

            if(!isExistFilePath(filePath))
            {
                QStandardItem *item = new QStandardItem(filePath);
                standardItemModel->appendRow(item);
            }
        }

        QModelIndex index = standardItemModel->index(standardItemModel->rowCount() - 1, 0);
        ui->listView->setCurrentIndex(index);
        ui->listView->scrollToBottom();
    }
}

void Widget::addFolderActionMenuItem(void)
{
    loadFile(false);
}

void Widget::addRecursiveFolderActionMenuItem(void)
{
    loadFile(true);
}

void Widget::clearActionMenuItem(void)
{
    standardItemModel->clear();
}

void Widget::setFormatCodeFile(QString formatfile, int index)
{
    QStandardItem *item = new QStandardItem(formatfile.trimmed());

    if(QString::compare(formatfile.left(3), "格式化", Qt::CaseInsensitive) == 0)
    {
        QLinearGradient linearGrad(QPointF(0, 0), QPointF(200, 200));
        linearGrad.setColorAt(0, Qt::darkGreen);
        linearGrad.setColorAt(1, Qt::yellow);
        QBrush brush(linearGrad);
        item->setBackground(brush);
    }
    standardItemModel->setItem(index, item);
}

void Widget::formatCode()
{
    QString exeApp;
    exeApp = QCoreApplication::applicationDirPath();
    exeApp+="/AStyle.exe";

    QFileInfo fileInfo(exeApp);
    if(!fileInfo.exists())
    {
        QMessageBox::about(NULL, "Warning", "Sorry, the files cannot be format because part of it is missing.<br/>"
                                            "You need \"Astyle.exe\" file to run, AStyle 3.0.1<br/>"
                                            "<font color='blue', font size=4>"
                                                "<u>"
                                                    "<a href=\"http://astyle.sourceforge.net/\" target=\"_blank\" title=\"AStyle\">http://astyle.sourceforge.net/</a>"
                                                    "<br/>"
                                                    "<a href=\"https://github.com/to9\" target=\"_blank\" title=\"CodeStyle\">https://github.com/to9</a>"
                                                "</u>"
                                            "</font>");
        return;
    }

    if(standardItemModel->rowCount() > 0)
    {
        QStringList fileList;
        for(int i = 0; i<standardItemModel->rowCount(); i++)
        {
            QString filePath = standardItemModel->item(i)->text();
            fileList.append(filePath.trimmed());
        }
        m_thread->processFormatCode(fileList, m_astyleConfig);
        m_thread->start();
    }
}


void Widget::loadFile(bool rfind)
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("选择代码根目录："),
                                                    "/home",
                                                    QFileDialog::ShowDirsOnly
                                                    | QFileDialog::DontResolveSymlinks);

    if(!dir.isEmpty())
    {
        QStringList fileslist;
        FindFiles(dir, fileslist, rfind);

        for(int i = 0; i < fileslist.size(); ++i)
        {
            QString filePath = fileslist.at(i);

            if(!isExistFilePath(filePath))
            {
                QStandardItem *item = new QStandardItem(filePath);
                standardItemModel->appendRow(item);
            }
        }

        QModelIndex index = standardItemModel->index(standardItemModel->rowCount() - 1, 0);
        ui->listView->setCurrentIndex(index);
        ui->listView->scrollToBottom();
    }
}

bool Widget::FindFiles(const QString &path, QStringList &fileslist, bool rfind)
{
    QDir dir(path);
     if (!dir.exists())
     return false;
     dir.setFilter(QDir::Dirs|QDir::Files);
     dir.setSorting(QDir::DirsFirst);
     QFileInfoList list = dir.entryInfoList();
     int i=0;
    do{
        QFileInfo fileInfo = list.at(i);
        if(fileInfo.fileName()=="." || fileInfo.fileName()=="..")
        {
            i++;
            continue;
        }

        bool bisDir=fileInfo.isDir();
        if(bisDir && rfind)
        {
            FindFiles(fileInfo.filePath(), fileslist, rfind);
        }
        else
        {
            if((QString(fileInfo.suffix()).toLower() == "c") ||
                    (QString(fileInfo.suffix()).toLower() == "h") ||
                    (QString(fileInfo.suffix()).toLower() == "cpp"))
            {
                QString file;
                file.append(fileInfo.path()).append("/").append(fileInfo.fileName());
                file.replace("/","\\");//windows
                file.prepend("        ");
                fileslist.append(file);
            }
        }
        i++;
    }while(i<list.size());
      return true;
}
