#ifndef CLIENTWND_H
#define CLIENTWND_H

#include <QObject>
#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QDialog>
#include <QPushButton>
#include <QListWidget>
#include <QListWidgetItem>
#include <QBrush>
#include <QThread>
#include <QKeyEvent>
#include <QPointer>

#include "mesclient.h"
#include "clientserver.h"


namespace Ui {
    class ClientWnd;
}

class ClientWnd : public MesClient
{
    Q_OBJECT
public:
    ClientWnd(const std::string& strHost, const unsigned short nPort, QWidget *parent = nullptr);
    ~ClientWnd();

private:
    Ui::ClientWnd * ui;
    QStringListModel friendModel;

    QDialog* logForm;
    QLineEdit* loginLE;
    QLineEdit* passwordLE;

    QString contactIp;

    QPointer<ClientServer>  pClientServer;
    QThread * pClientServerThread;

    void keyPressEvent(QKeyEvent *event);
    void setFriendModel();
    void checkUserStatus(const QString&);//true = online

signals:
    void sendMsg(const QString &ip, const QString &msg);

private slots:
    void slotShowSysMsg();
    void login();
    void enterLogData();
    void selectContact(QListWidgetItem*);
    void sendButtonClicked();
    void readMsg(int id, const QString&);
};

#endif // CLIENTWND_H
