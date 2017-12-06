#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

#include <QMainWindow>
#include <QString>
#include <QDir>
#include <QEvent>
#include <QCloseEvent>
#include <QMessageBox>
#include <QTableWidgetItem>
#include <QAbstractItemModel>
#include <QApplication>
#include <QMenu>
#include <QDesktopServices>
#include <QUrl>
#include <QVector>
#include <QSharedPointer>
#include <memory>
#include <functional>
#include <type_traits>
#include <qfuturewatcher.h>

class Export;
class ContextMenu;
class ChangeLanguage;


//for DLL
#include <windows.h>
#include "../include/FFParser_DLL.h"

using namespace FFParser;

typedef IStorageFactory* (CALL *GetStorageFunc)();


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


    using LibGuard = std::unique_ptr<std::remove_pointer<HMODULE>::type, std::function<void(HMODULE)>>;
    using recordPtr = QSharedPointer<IRecordsStream>;

    QStringList getProfiles();
    const recordPtr & getCurrentPtr();
    size_t& getCurrentStep();
    size_t& getCurrentTotalRecords();
    void updateTotalRecords();
    const recordPtr & getPtr(ERecordTypes type, size_t profile);
    ERecordTypes getTabTypeByIndex(size_t index);
    ERecordTypes getCurrentTabType();
    QString getTableField(const char * text);
    IDataExporter * getExporter();
    void exportSelectedFile(const char * path, bool md5);
    void exportAndOpenSelected();
    void showSelectedFileInExplorer();
    QTableWidgetItem* getLastClickedItem();
    QFutureWatcher<void>* getWatcher(size_t profile, ERecordTypes rectype);
    QFutureWatcher<void>* getCurrentWatcher();


signals:
    void libErrorSignal(const QString& error_text, const QString& error_title);
    void recordsLoadedSignal();

private slots:
    void onLibErrorSlot(const QString& error_text, const QString& error_title);
    void onRecordsLoadFinished();

protected:
    void changeEvent(QEvent *);
    void closeEvent(QCloseEvent *);

    void setTableHeaders();
    void loadNewRecords();
    void viewRecords();
    void setNameProfile();
    void viewCounterRecords(size_t first, size_t last);
    void search();

protected slots:
    void slotLanguageChanged(QAction *);


private slots:
    void on_setRecordButton_clicked();

    void on_nextPageButton_clicked();

    void on_prevPageButton_clicked();

    void on_searchButton_clicked();

    void on_chooseProfile_activated(int index);

    void on_tabWidget_currentChanged(int index);

    void on_clearSearchRecord_clicked();

    void slotCustomMenuRequested(QPoint pos);
    void slotCloseContextMenu();
    void slotOpenSelectedFileMenu();
    void slotExportSelectedFile();
    void slotShowInExplorer();
    void slotMenuExport();
    void slotExitProgram();
    void slotAboutMenu();
    void slotOpenCacheFolder();
    void slotOpenProfileFolder();

    void on_tableWidget_itemSelectionChanged();
    void on_tableWidget_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous);

    void on_lineEdit_returnPressed();

private:
    //dll load
    const wchar_t* dllname;
    LibGuard dll_load;

    static void s_libErrorHandler(const char* error_text, const char* error_title);
    static void s_asyncRecordsLoading(MainWindow* mw, IRecordsStream* stream, size_t step);

    //lib storage
    IStorageFactory* DLLStorage;
    const QString tempDirPath;
    QDir tempDir;

    Ui::MainWindow ui;
    Export *_exportFileWindow;
    ContextMenu *_menu;
    ChangeLanguage *_changeLanguage;

    void initialLoad();
    void createTableUI();

    void createLanguageMenu(void);
    void createFileMenu();
    void createHelpMenu();

    void setMainFormEnabled(bool);

    struct ProfileRecordData
    {
        recordPtr historyRecord;
        recordPtr bookmarksRecord;
        recordPtr loginsRecord;
        recordPtr cacheRecord;
    };

    QVector<ProfileRecordData> _allProfiles;

    QTableWidgetItem *_lastClickedItem;

    size_t _firstRecord;
    size_t _profileNumber;
    size_t _allAmountProfile;
    bool _searchFlag;

    QVector<size_t> _stepForTabs;
    QVector<QVector<size_t>> _totalRecordsArray;

    //for concurrency
    QVector<QVector<QFutureWatcher<void>*>> _watchers;
};

#endif // MAINWINDOW_H
