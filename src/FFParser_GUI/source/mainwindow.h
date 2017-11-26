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
#include <vector>

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
    void viewRecord(IRecordsStream *);
    void removeRowTable(size_t);
    void switchViewRecords(size_t);
    void setNameProfile();


protected slots:
    void slotLanguageChanged(QAction *);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_pushButton_3_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_4_clicked();

    void on_comboBox_activated(int index);

    void on_tabWidget_currentChanged(int index);

private:
    Ui::MainWindow ui;
    void createUI(const QStringList &headers, size_t);

    void loadLanguage(const QString &rLanguage);

    void createLanguageMenu(void);

    void initialLoadRecord(IRecordsStream *);
    void loadNewNextRecords(const size_t &, const size_t &);
    void checkNewRecords(const size_t &, const size_t &, const size_t &);
    bool isOutOfRange(const size_t &, const size_t &, const size_t &);
    bool checkRecords(const size_t &);
    void viewStep(const size_t &);

    QTranslator m_translator;
    QTranslator m_translatorQt;
    QString m_currLang;
    QString m_langPath;



    IRecordsStream *historyRecord;
    IRecordsStream *bookmarksRecord;
    IRecordsStream *loginRecord;
    IRecordsStream *cacheRecord;

    size_t _firstRecord;
    size_t oldStep;
    size_t profileNumber;
    size_t m_allAmountProfile;

    std::vector<size_t> stepForTabs;

    //dll load
    const wchar_t* dllname;
    HINSTANCE dll_load;
    //global access to storage
    IStorageFactory* DLLStorage;



};

#endif // MAINWINDOW_H
