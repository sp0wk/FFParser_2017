#ifndef CONTEXTMENU_H
#define CONTEXTMENU_H

#include <QMenu>
#include <QDebug>
#include "mainwindow.h"

class ContextMenu : public QMenu
{
    Q_OBJECT

public:
    explicit ContextMenu(MainWindow *parent);
    ~ContextMenu() = default;

    virtual void popup(const QPoint &pos, QAction *at = nullptr)
    {
        _menu.popup(pos, at);
    }

public slots:
    void slotOpenFile();
    void slotOpenUrl();
    void slotExport();

private:
    void leaveEvent(QEvent *);

    MainWindow *_mainwindow;
    QMenu _menu;
};


#endif // CONTEXTMENU_H
