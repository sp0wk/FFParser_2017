#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QTranslator>
#include <QDir>
#include <QEvent>
#include <QLocale>
#include <QCloseEvent>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QAbstractItemModel>

//for DLL
#include <windows.h>
#include "../include/FFParser_DLL.h"

using namespace FFParser;

typedef IStorageFactory* (CALL *GetStorageFunc)();





#include "ui_mainwindow.h"


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

protected:
    void changeEvent(QEvent *);
    void closeEvent(QCloseEvent *);


    void setNameColumnTable(IRecordsStream *);
    void veiwRecord(IRecordsStream *);
    void removeRowTable(size_t);
    void switchVeiwRecords(size_t);

protected slots:
    void slotLanguageChanged(QAction *);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_tabWidget_tabBarClicked(int index);

    void on_pushButton_3_clicked();


    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow ui;
    void createUI(const QStringList &headers, size_t);

    void loadLanguage(const QString &rLanguage);

    void createLanguageMenu(void);

    QTranslator m_translator;
    QTranslator m_translatorQt;
    QString m_currLang;
    QString m_langPath;



    IRecordsStream *historyRecord;
    IRecordsStream *bookmarksRecord;

    size_t firstRecord;
    size_t lastRecord;
    size_t step;
    size_t oldStep;
    size_t flag;
    size_t counterRecords;

    //dll load
    const wchar_t* dllname;
    HINSTANCE dll_load;
    //global access to storage
    IStorageFactory* DLLStorage;



};

#endif // MAINWINDOW_H
