#include "mesclient.h"

MesClient::MesClient(const std::string& strHost, const unsigned short & nPort, QWidget *parent):
    QMainWindow(parent)
{
    context_ptr = std::make_shared<io_context>();
    resolver_ptr = std::make_shared<resolver>(*context_ptr);
    query qry(strHost, boost::lexical_cast<std::string>(nPort));
    end_point = *resolver_ptr->resolve(qry);
    sockForSrv_ptr = std::make_shared<boost_socket>(*context_ptr);
    context_ptr->post(boost::bind(&MesClient::start, this));
    srv_thread = std::make_shared<boost::thread>(boost::bind(&io_context::run, context_ptr));
}

MesClient::~MesClient(){
    if (not frLstFile.isNull()){
        if (frLstFile->isOpen())
            frLstFile->close();
        delete frLstFile;
    }
}

std::ostream& operator<< (std::ostream &out, MesClient::MsgForSend & msg){
    unsigned char ch[2];
    ch[0] = msg.msgSize;
    ch[1] = msg.msgSize >> 8;

    out << ch[0] << ch[1] << msg.text;
    return out;
}

std::istream& operator>> (std::istream &in, MesClient::MsgForSend & msg){
    unsigned char ch[2];
    in >> ch[0] >> ch[1] >> msg.text;
    msg.msgSize = (ch[0] | (ch[1] << 8));
    return in;
}

void MesClient::start(){
    connectToServer();
    readFromServer();
}

void MesClient::stop(){
    sockForSrv_ptr->shutdown(boost_socket::shutdown_both);
    context_ptr->stop();
}

//private
bool MesClient::connectToServer(){
    if (!context_ptr->stopped()){
        context_ptr->restart();
    }
    sockForSrv_ptr->connect(end_point, last_error_code);
    if (!last_error_code)
        return true;
    else {
        sysMsg = QString::fromStdString(last_error_code.message());
        emit showSysMsg();
        return false;
    }
}

void MesClient::slotSendToServer(QString message){
    MsgForSend msgForServer;
    std::string stringMsg = message.toStdString();
    msgForServer(stringMsg, static_cast<unsigned short>(stringMsg.length()) );
    boost::asio::streambuf buff;
    std::ostream out(&buff);
    out << msgForServer;
    sockForSrv_ptr->write_some(buff.data(), last_error_code);

    if (last_error_code){
        return;
    }
    else{
        //readFromServer();
    }
}

void MesClient::readFromServer(){
    try {
        for (;;){
            unsigned short size;
            std::size_t bytesRead = 0;
            bytesRead = sockForSrv_ptr->read_some(boost::asio::buffer(&size, sizeof (unsigned short) ) );
            if (bytesRead){
                std::string msgFromServer;
                sockForSrv_ptr->read_some(boost::asio::buffer(&msgFromServer, size ) );
                //emit srvMsgAccepted(QString::fromStdString(msgFromServer));
            }
        }
    } catch (boost::exception const& exc) {
        std::cout << dynamic_cast<std::exception const&>(exc).what() << "\n";
        stop();
    }

}


void MesClient::slotConnected(){
    sysMsg = "Connected";
    emit showSysMsg();
}

void MesClient::slotReconnect(){
    srv_thread = std::make_shared<boost::thread>(std::bind(&MesClient::start, this));
}

void MesClient::slotDisconnect(){
    stop();
}

bool MesClient::readFriendList(){

    frLstFile = new QFile("./data/frLst.data");
    if (frLstFile->exists()){
        readFriendList();
    }
    delete frLstFile;

    frLstFile->open(QFile::ReadOnly);
    char buff[1024];
    QStringList buffLst;
    while (frLstFile->readLine(buff, sizeof(buff)) > 0) {
        //friendLst.append(QString(buff).trimmed().split(' '));
        buffLst = QString(buff).trimmed().split(' ');
        friendLst.insert(buffLst[0], buffLst[1]);
    }

    return true;
}

void MesClient::slotError(QAbstractSocket::SocketError){}
