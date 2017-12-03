#ifndef EXPORTCACHEFILEDIALOG_H
#define EXPORTCACHEFILEDIALOG_H

#include <QDialog>
#include "mainwindow.h"

namespace Ui {
class ExportCacheFileDialog;
}

class ExportCacheFileDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ExportCacheFileDialog(MainWindow *parent);
    ~ExportCacheFileDialog();

    void exportCacheFile();

private slots:
    void on_browseButton_clicked();

    void on_exportButton_clicked();

private:
    Ui::ExportCacheFileDialog *ui;

    MainWindow *_mainwindow;


};

#endif // EXPORTCACHEFILEDIALOG_H
