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
#include <QVector>
#include <QSharedPointer>
#include <memory>
#include <functional>
#include <type_traits>


class Export;
class ContextMenu;


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

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();


    using LibGuard = std::unique_ptr<std::remove_pointer<HMODULE>::type, std::function<void(HMODULE)>>;
    using recordPtr = QSharedPointer<IRecordsStream>;

    const recordPtr & getPtrByTabIndex(size_t);
    const recordPtr & getPtr(ERecordTypes, size_t);
    QString getTableField(const char *);
    IDataExporter * getExporter();
    void exportSelectedFile(const char *, bool);

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

    void on_chooseProfile_currentIndexChanged(int index);

    void on_tabWidget_currentChanged(int index);

    void on_clearSearchRecord_clicked();

    void slotCustomMenuRequested(QPoint pos);
    void slotCloseContextMenu();
    void slotMenuExport();

private:
    //dll load
    const wchar_t* dllname;
    LibGuard dll_load;
    //lib storage
    IStorageFactory* DLLStorage;
    const QString tempDir;

    Ui::MainWindow ui;
    Export *_exportFileWindow;
    ContextMenu *_menu;

    void createUI(const QStringList &headers, size_t);

    void loadLanguage(const QString &rLanguage);

    void createLanguageMenu(void);
    void createFileMenu();

    bool initialLoadRecord(const recordPtr &);
    void checkNewRecords(size_t, size_t, size_t);
    bool isOutOfRange(size_t, size_t, size_t);
    void viewStep(size_t);
    void viewCounterRecords(size_t, size_t, const recordPtr &);
    void search();


    struct ProfileRecordData
    {
        recordPtr historyRecord;
        recordPtr bookmarksRecord;
        recordPtr loginsRecord;
        recordPtr cacheRecord;
    };

    QVector<ProfileRecordData> _allProfiles;

    QTranslator m_translator;
    QTranslator m_translatorQt;
    QString m_currLang;
    QString m_langPath;

    size_t _firstRecord;
    size_t oldStep;
    size_t _profileNumber;
    size_t _allAmountProfile;
    bool _searchFlag;

    std::vector<size_t> stepForTabs;
};

#endif // MAINWINDOW_H
