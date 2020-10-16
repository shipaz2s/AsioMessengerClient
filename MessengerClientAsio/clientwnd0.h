#ifndef CLIENTWND_H
#define CLIENTWND_H

#include <QMainWindow>
#include "mesclient.h"

namespace Ui {
    class ClientWnd;
}

class ClientWnd : public QMainWindow, MesClient
{
    Q_OBJECT
public:
    explicit ClientWnd(const QString& strHost, int nPort, QObject* clientParent = nullptr, QWidget *parent = nullptr);
    ~ClientWnd();

private:
    Ui::ClientWnd *ui;

signals:

};

#endif // CLIENTWND_H
