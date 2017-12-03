#ifndef EXPORT_H
#define EXPORT_H

#include <QDialog>
#include <QDebug>
#include <QString>
#include <QVector>
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

    void setProfile(const QString &);
    void addProfileCombobox();

    void exportData();

private slots:


private:
    Ui::Export *ui;
    MainWindow *_mainwindow;
    QVector<QString> _profiles;

};

#endif // EXPORT_H
