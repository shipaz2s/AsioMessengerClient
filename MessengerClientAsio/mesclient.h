#ifndef MESCLIENT_H
#define MESCLIENT_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QtCore>
#include <QPointer>

//boost
#include <boost/asio.hpp>
#include <boost/thread.hpp>
//#include <boost/thread/mutex.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/exception/all.hpp>

//std
#include <memory>
#include <iostream>
#include <string>
#include <fstream>


class MesClient : public QMainWindow
{
    typedef boost::asio::io_context io_context;
    typedef std::shared_ptr<boost::asio::io_context> io_context_ptr;
    typedef boost::asio::ip::tcp::endpoint endpoint;
    typedef boost::asio::ip::tcp::acceptor acceptor;
    typedef std::shared_ptr<boost::asio::ip::tcp::acceptor> acceptor_ptr;
    typedef boost::asio::ip::tcp::resolver resolver;
    typedef boost::asio::ip::tcp::resolver::query query;
    typedef std::shared_ptr<boost::asio::ip::tcp::socket> socket_ptr;
    typedef boost::asio::ip::tcp::socket boost_socket;
    typedef boost::system::error_code error_code;
    typedef std::shared_ptr<std::string> string_ptr;

    Q_OBJECT
protected:

    QString sysMsg;
    QMap<QString,QString> friendLst;
    QPointer<QFile> frLstFile;

    unsigned short srvPort;
    std::string srvIp;

    error_code last_error_code;
    io_context_ptr context_ptr;
    std::shared_ptr<boost::thread> thread;
    std::shared_ptr<resolver> resolver_ptr;
    socket_ptr sockForSrv_ptr;
    endpoint end_point;

    std::shared_ptr<boost::thread> srv_thread;


public:
    struct MsgForSend {
        unsigned short msgSize;
        std::string text;

        MsgForSend(std::string txt = "", unsigned short size = 0):
            msgSize(size), text(txt)
        {
        }

        void operator() (std::string txt, unsigned short size){
            text = txt;
            msgSize = size;
        }

        friend std::ostream& operator<< (std::ostream &out, MsgForSend & msg);
        friend std::istream& operator>> (std::istream &in, MsgForSend & msg);

    };

public:
    MesClient(const std::string& strHost, const unsigned short & nPort, QWidget *parent = 0);
    ~MesClient();
    void start();
    void stop();

protected:
    bool readFriendList();
    bool connectToServer();
    void readFromServer();

signals:
    void signalSendToServer(QString);
    void showSysMsg();//определить для него слот в наследуемом классе виджета
    void signalConnected();
    void signalDisconnected();
    void srvMsgAccepted(QString);//определить для него слот в наследуемом классе виджета

protected slots:
    void slotError(QAbstractSocket::SocketError);
    void slotSendToServer(QString);
    void slotConnected();
    void slotReconnect();
    void slotDisconnect();
};

#endif // MESCLIENT_H
