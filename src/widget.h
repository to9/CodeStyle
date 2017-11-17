#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>

class ProcessThread;
class QListView;
class QStandardItemModel;
class QSettings;

namespace Ui {
class Widget;
}

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = 0);
    ~Widget();

    bool FindFiles(const QString &path, QStringList &fileslist, bool rfind);

protected:
     void closeEvent(QCloseEvent *event);

public slots:
    void loadFile(bool rfind = true);
    void formatCode();
    void setFormatCodeFile(QString, int);

    void showSelectMenu(const QPoint &pos);
    void deleteActionMenuItem(void);
    void addFileActionMenuItem(void);
    void addFolderActionMenuItem(void);
    void addRecursiveFolderActionMenuItem(void);
    void clearActionMenuItem(void);

private:
    bool isExistFilePath(QString &filePath);

private:
    Ui::Widget *ui;
    ProcessThread *m_thread;
    QStandardItemModel *standardItemModel;
    QSettings *m_settings;
    QString m_astyleConfig;
};

#endif // WIDGET_H
