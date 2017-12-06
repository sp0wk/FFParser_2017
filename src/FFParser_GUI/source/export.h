#ifndef EXPORT_H
#define EXPORT_H

#include <QDialog>
#include <QString>
#include <QVector>
#include <qfuturewatcher.h>
#include "mainwindow.h"


namespace Ui {
class Export;
}

class Export : public QDialog
{
    Q_OBJECT

public:
    explicit Export(MainWindow *parent);
    ~Export();

    void exportData();
    virtual void show();

protected:
    void closeEvent(QCloseEvent *event);

private slots:

    void on_exportButton_clicked();

    void on_browseButton_clicked();

private:
    Ui::Export *ui;
    MainWindow *_mainwindow;

    QFutureWatcher<void>* _watcher;
};

#endif // EXPORT_H
