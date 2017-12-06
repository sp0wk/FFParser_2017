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

signals:
    void streamExportFinished();

private slots:

    void on_exportButton_clicked();

    void on_browseButton_clicked();

    void onExportFinishedSlot();

private:
    Ui::Export *ui;
    MainWindow *_mainwindow;
    IDataExporter* exportPtr;

    QVector<QFutureWatcher<void>*> _watchers;

    static void s_exportRecords(Export* exp, IRecordsStream* rstream, const QString& path, bool md5);
    static void s_exportCache(Export* exp, IRecordsStream* rstream, const QString& path, size_t profile, bool md5);
};

#endif // EXPORT_H
