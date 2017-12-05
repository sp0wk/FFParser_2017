#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainwindow.h"

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
#include <QVector>
#include <QSharedPointer>
#include <memory>
#include <functional>
#include <type_traits>


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
    const recordPtr & getPtrByTabIndex(size_t);
    const recordPtr & getPtr(ERecordTypes, size_t);
    ERecordTypes getCurrentTabType();
    QString getTableField(const char *);
    IDataExporter * getExporter();
    void exportSelectedFile(const char *, bool);
    void exportAndOpenSelected();
    void showSelectedFileInExplorer();
    QTableWidgetItem* getLastClickedItem();

protected:
    void changeEvent(QEvent *);
    void closeEvent(QCloseEvent *);

    void setNameColumnTable(const recordPtr &);
    void viewRecord(const recordPtr &);
    void removeRowTable(size_t);
    void switchViewRecords(size_t);
    void setNameProfile();

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

private:
    //dll load
    const wchar_t* dllname;
    LibGuard dll_load;
    //lib storage
    IStorageFactory* DLLStorage;
    const QString tempDirPath;
    QDir tempDir;

    Ui::MainWindow ui;
    Export *_exportFileWindow;
    ContextMenu *_menu;
    ChangeLanguage *_changeLanguage;

    void createUI(const QStringList &headers, size_t);

    void createLanguageMenu(void);
    void createFileMenu();
    void createHelpMenu();

    void initialLoadRecord(const recordPtr &);
    void checkNewRecords(size_t, size_t, size_t);
    bool isOutOfRange(size_t, size_t, size_t);
    void viewStep(size_t);
    void viewCounterRecords(size_t, size_t, const recordPtr &);
    void search();
    void setMainFormEnabled(bool);
    void initialLoad();


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
    size_t _oldStep;
    size_t _profileNumber;
    size_t _allAmountProfile;
    bool _searchFlag;

    std::vector<size_t> stepForTabs;
};

#endif // MAINWINDOW_H
