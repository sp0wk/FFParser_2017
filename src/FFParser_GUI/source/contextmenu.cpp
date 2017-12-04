#include "contextmenu.h"
#include "exportcachefiledialog.h"
#include <QString>
#include <QClipboard>


ContextMenu::ContextMenu(MainWindow *parent)
    : _menu(parent),
      _mainwindow(parent)
{

    QAction *copyText = new QAction(trUtf8("Copy"), this);
    QAction *openUrl = new QAction(trUtf8("Open URL"), this);
    QAction *openFile = new QAction(trUtf8("Open File"), this);
    QAction *exports = new QAction(trUtf8("Export"), this);
    QAction *showExpl = new QAction(trUtf8("Show in explorer"), this);

    connect(copyText, SIGNAL(triggered(bool)), this, SLOT(slotCopyText()));
    connect(openUrl, SIGNAL(triggered(bool)), this, SLOT(slotOpenUrl()));
    connect(openFile, SIGNAL(triggered(bool)), this, SLOT(slotOpenFile()));
    connect(exports, SIGNAL(triggered(bool)), this, SLOT(slotExport()));
    connect(showExpl, SIGNAL(triggered(bool)), this, SLOT(slotShowInExplorer()));

    _menu.addAction(copyText);
    _menu.addAction(openUrl);
    _menu.addAction(openFile);
    _menu.addAction(exports);
    _menu.addAction(showExpl);
}

void ContextMenu::leaveEvent(QEvent *event)
{
    _menu.hide();
}


void ContextMenu::popup(const QPoint &pos, QAction *at)
{
    if (_mainwindow->getCurrentTabType() != ERecordTypes::CACHEFILES) {
        _menu.actions().at(2)->setDisabled(true);
        _menu.actions().at(3)->setDisabled(true);
        _menu.actions().at(4)->setDisabled(true);
    }
    else {
        _menu.actions().at(2)->setEnabled(true);
        _menu.actions().at(3)->setEnabled(true);
        _menu.actions().at(4)->setEnabled(true);
    }

    _menu.popup(pos, at);
}


void ContextMenu::slotCopyText()
{
    QApplication::clipboard()->setText(_mainwindow->getLastClickedItem()->text());
}

void ContextMenu::slotOpenFile()
{
    _mainwindow->exportAndOpenSelected();
}


void ContextMenu::slotShowInExplorer()
{
    _mainwindow->showSelectedFileInExplorer();
}


void ContextMenu::slotOpenUrl()
{
    QString found = _mainwindow->getTableField("url");
    if (found == "")
        found = _mainwindow->getTableField("hostname");

    QUrl temp = found;

    if (!QDesktopServices::openUrl(temp))
    {
        QMessageBox::warning(_mainwindow, tr("Warning"),
                             tr("Cannot open URL\n"),
                             QMessageBox::Ok);
    }
}

void ContextMenu::slotExport()
{
    ExportCacheFileDialog exportCache(_mainwindow);
    exportCache.exec();
}




