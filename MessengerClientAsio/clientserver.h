#ifndef CLIENTSERVER_H
#define CLIENTSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QByteArray>

class ClientServer: public QObject
{
    Q_OBJECT
    QTcpServer* clientServer;
    QTcpSocket* pClientSocket;
    quint16 nextBlockSize;
    int ownId;
public:
    ClientServer(int id);
    ~ClientServer();

protected:
    void answerToClient(QTcpSocket*, const QString&);//looks like no need to use...

signals:
    void sentToClient(int id);//уже не помню зачем.. вероятно, вовсе не нужен.
    void readMsg(int id, const QString&);

public slots:
    void slotNewConnection();
    void slotReadClient();
    void sendMsg(const QString &ip, const QString &msg);
};

#endif // CLIENTSERVER_H
