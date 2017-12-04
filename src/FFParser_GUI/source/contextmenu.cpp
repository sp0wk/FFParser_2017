#include "contextmenu.h"
#include "exportcachefiledialog.h"
#include <QString>


ContextMenu::ContextMenu(MainWindow *parent)
    : _menu(parent),
      _mainwindow(parent)
{

    QAction *openFile = new QAction(trUtf8("Open File"), this);
    QAction *openUrl = new QAction(trUtf8("Open URL"), this);
    QAction *exports = new QAction(trUtf8("Export"), this);

    connect(openFile, SIGNAL(triggered(bool)), this, SLOT(slotOpenFile()));
    connect(openUrl, SIGNAL(triggered(bool)), this, SLOT(slotOpenUrl()));
    connect(exports, SIGNAL(triggered(bool)), this, SLOT(slotExport()));

    _menu.addAction(openFile);
    _menu.addAction(openUrl);
    _menu.addAction(exports);
}

void ContextMenu::leaveEvent(QEvent *event)
{
    _menu.hide();
}


void ContextMenu::slotOpenFile()
{
    QUrl temp = QUrl::fromLocalFile(_mainwindow->getTableField("path"));
    if (!QDesktopServices::openUrl(temp))
    {
        QMessageBox::warning(_mainwindow, "Warning",
                             tr("This is not path file!\n"),
                             QMessageBox::Ok);
    }
}

void ContextMenu::slotOpenUrl()
{
    QString found = _mainwindow->getTableField("url");
    if (found == "")
        found = _mainwindow->getTableField("hostname");

    QUrl temp = found;

    if (!QDesktopServices::openUrl(temp))
    {
        QMessageBox::warning(_mainwindow, "Warning",
                             tr("This is not URL address!\n"),
                             QMessageBox::Ok);
    }
}

void ContextMenu::slotExport()
{
    ExportCacheFileDialog exportCache(_mainwindow);
    exportCache.exec();
}




