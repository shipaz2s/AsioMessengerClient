#include "clientwnd.h"
#include "ui_clientwnd1.h"

ClientWnd::ClientWnd(const QString& strHost, int nPort, QObject* clientParent, QWidget *parent)
    : QMainWindow(parent),
      MesClient(strHost, nPort, clientParent),
      ui(new Ui::ClientWnd)
{
}
