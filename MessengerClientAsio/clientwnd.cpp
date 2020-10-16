#include "clientwnd.h"
#include "ui_clientwnd.h"

ClientWnd::ClientWnd(const std::string& strHost, const unsigned short nPort, QWidget *parent):
    MesClient(strHost, nPort, parent),
    ui(new Ui::ClientWnd)
{
    ui->setupUi(this);
    connect(this, SIGNAL(showSysMsg()), SLOT(slotShowSysMsg()) );
    connect(ui->action_Connect, SIGNAL( triggered() ), this, SLOT( slotReconnect() ) );
    connect(ui->action_Login, SIGNAL(triggered()), this, SLOT(login()));
    connect(ui->action_Log_out, SIGNAL(triggered()), this, SLOT(slotDisconnect()) );
    connect(ui->sendButton, SIGNAL(clicked()), this, SLOT(sendButtonClicked()));

    slotShowSysMsg();
    setFriendModel();


    connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(selectContact(QListWidgetItem*)));

    pClientServerThread = new QThread();
    //p
}

ClientWnd::~ClientWnd(){
    delete ui;
    if (!pClientServer.isNull())
        delete pClientServer;
    pClientServerThread->quit();
    pClientServerThread->wait();
    delete pClientServerThread;
}


void ClientWnd::setFriendModel(){
    if (not friendLst.isEmpty()){
        QMap<QString, QString>::iterator iter = friendLst.begin();
        QStringList inModel;
        while (iter != friendLst.end()){
            inModel << iter.value();
            ++iter;
        }
        ui->listWidget->addItems(inModel);
    }
}

void ClientWnd::checkUserStatus(const QString & id){
    slotSendToServer("2 " + id);
}

//slots
void ClientWnd::slotShowSysMsg(){
    ui->statusbar->showMessage(sysMsg);
    QStringList msg;
    msg = sysMsg.trimmed().split(' ');
    if (msg.length()>1){
        switch (msg[1].toInt()) {
        case 1:
            if (msg[2] != '0'){
                sysMsg = "Login succeeded.";
                pClientServer = new ClientServer(msg[2].toInt());
                pClientServer->moveToThread(pClientServerThread);
                connect(this, SIGNAL(sendMsg(const QString &, const QString &)),
                        pClientServer, SLOT(sendMsg(const QString &, const QString &)) );
                connect(pClientServer, SIGNAL(readMsg(int, const QString &)),
                        this, SLOT(readMsg(int, const QString &)));
                pClientServerThread->start();
            }
            else
                sysMsg = "Login unsucceeded!";
            slotShowSysMsg();
            break;
        case 2:
            if (msg[3] == "NULL"){
                ui->listWidget->currentItem()->setBackground(Qt::white);
                ui->listWidget->currentItem()->setForeground(Qt::black);
                contactIp = "NULL";
            }
            else{
                ui->listWidget->currentItem()->setBackground(Qt::darkGreen);
                ui->listWidget->currentItem()->setForeground(Qt::white);
                contactIp = msg[3];
            }
            break;
        default:

            break;
        }

    }
}

void ClientWnd::login(){
    logForm = new QDialog(this);
    QVBoxLayout mainLayout;
    mainLayout.addWidget(new QLabel("Login", logForm));
    loginLE = new QLineEdit(logForm);
    mainLayout.addWidget(loginLE);
    mainLayout.addWidget(new QLabel("Password", logForm));
    passwordLE = new QLineEdit(logForm);
    passwordLE->setEchoMode(QLineEdit::Password);
    mainLayout.addWidget(passwordLE);
    QPushButton* okBtn = new QPushButton("&Ok", logForm);
    mainLayout.addWidget(okBtn);
    connect(okBtn, SIGNAL(clicked()), this, SLOT( enterLogData() ) );

    logForm->setLayout(&mainLayout);
    logForm->setWindowFlag(Qt::MSWindowsFixedSizeDialogHint);
    logForm->setWindowModality(Qt::WindowModal);

    logForm->hide();
    logForm->show();
}

void ClientWnd::enterLogData(){
    slotSendToServer("1 "+loginLE->text()+" "+passwordLE->text());
    logForm->close();
    delete logForm;
}

void ClientWnd::selectContact(QListWidgetItem* item){
    QString contactName = item->data(Qt::DisplayRole).toString();
    QString id = friendLst.key(contactName);
    checkUserStatus(id);
}

void ClientWnd::sendButtonClicked(){
    QString msg = ui->msgEdit->toPlainText();
    ui->msgEdit->clear();
    ui->chatEdit->append("\n" + msg);
    emit sendMsg(contactIp, msg);
}

void ClientWnd::keyPressEvent(QKeyEvent *event){
    if ( (event->key() == Qt::Key_Return) and ui->msgEdit->hasFocus()){
        sendButtonClicked();
    }
    else
        QWidget::keyPressEvent(event);
}

void ClientWnd::readMsg(int id, const QString &str){
    ui->chatEdit->append(QString::number(id) + " " + str);
}
