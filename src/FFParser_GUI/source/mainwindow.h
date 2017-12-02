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
#include <QApplication>
#include <vector>
#include <QMenu>
#include <QDesktopServices>
#include <QUrl>
#include "export.h"

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
    void on_setRecordButton_clicked();

    void on_nextPageButton_clicked();

    void on_prevPageButton_clicked();

    void on_searchButton_clicked();

    void on_chooseProfile_activated(int index);

    void on_tabWidget_currentChanged(int index);

    void on_clearSearchRecord_clicked();

    void slotOpenFile();
    void slotOpenUrl();
    void slotExport();
    void slotCustomMenuRequested(QPoint pos);
    void slotMenuExport();

private:
    Ui::MainWindow ui;
    Export *exportFileWindow;

    void createUI(const QStringList &headers, size_t);

    void loadLanguage(const QString &rLanguage);

    void createLanguageMenu(void);
    void createFileMenu();

    bool initialLoadRecord(IRecordsStream *);
    void checkNewRecords(const size_t &, const size_t &, const size_t &);
    bool isOutOfRange(const size_t &, const size_t &, const size_t &);
    bool checkRecords(const size_t &);
    void viewStep(const size_t &);
    void viewCounterRecords(const size_t &, const size_t &, IRecordsStream *);
    void search();
    bool ptrIsNotNull(const size_t &);
    IRecordsStream *getPtr(const size_t &);
    const char *getColumnTableName(IRecordsStream *, const char*, const size_t &);

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
    size_t _profileNumber;
    size_t _allAmountProfile;
    bool _searchFlag;


    std::vector<size_t> stepForTabs;

    //dll load
    const wchar_t* dllname;
    HINSTANCE dll_load;
    //global access to storage
    IStorageFactory* DLLStorage;



};

#endif // MAINWINDOW_H
