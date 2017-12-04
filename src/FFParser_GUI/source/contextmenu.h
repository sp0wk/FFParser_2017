#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <QMenu>
#include <QDebug>
#include "mainwindow.h"
#include "../include/IDataExporter.h"

class ContextMenu : public QMenu
{
    Q_OBJECT

public:
    explicit ContextMenu(MainWindow *parent);
    ~ContextMenu() = default;

    virtual void popup(const QPoint &pos, QAction *at = nullptr);

public slots:
    void slotCopyText();
    void slotOpenFile();
    void slotOpenUrl();
    void slotExport();
    void slotShowInExplorer();

private:
    void leaveEvent(QEvent *);

    MainWindow *_mainwindow;
    QMenu _menu;

    IDataExporter *_exportPtr;
};


#endif // CONTEXTMENU_H
