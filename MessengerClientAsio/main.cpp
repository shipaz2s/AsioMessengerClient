#include <QApplication>
#include "clientwnd.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QFile hostfile("./data/host.txt");
    if (hostfile.exists()){
        hostfile.open(QFile::ReadOnly);
    }
    std::string host = hostfile.readLine().toStdString();
    //ClientWnd wnd(host,2323);
    ClientWnd wnd("127.0.0.1",2323);

    wnd.show();

    return a.exec();
}
