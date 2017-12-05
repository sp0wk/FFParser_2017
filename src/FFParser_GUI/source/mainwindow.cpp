#include "mainwindow.h"
#include <QVariant>
#include <QProcess>
#include <QtConcurrent/QtConcurrentRun>
#include <QFuture>
#include "contextmenu.h"
#include "export.h"
#include "exportcachefiledialog.h"
#include "changelanguage.h"


using recordPtr = MainWindow::recordPtr;
using SetErrorCallbackFunc = void (CALL *)(void (*callback) (const char* error_text, const char* error_title));

//lib error handler
void MainWindow::s_libErrorHandler(const char* error_text, const char* error_title)
{
    QMessageBox::warning(0, error_title, error_text, QMessageBox::Ok);
}


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    dllname(L"FFParser_DLL.dll"),
    dll_load(LoadLibrary(dllname), [](HMODULE dll) { if (dll) FreeLibrary(dll); }),
    tempDirPath(QCoreApplication::applicationDirPath() + "/temp/"),
    tempDir(tempDirPath),
    _exportFileWindow(new Export(this)),
    _menu(new ContextMenu(this)),
    _changeLanguage(new ChangeLanguage(this)),
    _firstRecord(0),
    _profileNumber(0),
    _allAmountProfile(0),
    _searchFlag(false),
    _stepForTabs(4, 25)
{
    ui.setupUi(this);
    createLanguageMenu();
    createFileMenu();
    createHelpMenu();
    createTableUI();

    //create folder for temporary files
    if (!tempDir.exists()) {
        tempDir.mkdir(tempDirPath);
    }

    QString appPath = QApplication::applicationDirPath();
    QIcon icon(appPath.append("/FFParser_GUI.ico"));
    this->setWindowIcon(icon);

    initialLoad();
}

MainWindow::~MainWindow()
{
    delete _exportFileWindow;
    delete _menu;

    //delete temp dir
    tempDir.removeRecursively();
}

void MainWindow::initialLoad()
{
    if (dll_load.get() != nullptr)
    {
        GetStorageFunc dll_getstorage = (GetStorageFunc) GetProcAddress(dll_load.get(), "GetStorage");
        SetErrorCallbackFunc dll_setErrorCB = (SetErrorCallbackFunc) GetProcAddress(dll_load.get(), "SetErrorCallback");

        //set error callback
        dll_setErrorCB(MainWindow::s_libErrorHandler);

        DLLStorage = dll_getstorage();

        _allAmountProfile = DLLStorage->getNumberOfProfiles();

        if (_allAmountProfile == 0)
        {
            setMainFormEnabled(false);
        }
        else
        {
            setNameProfile();

            _allProfiles.resize(_allAmountProfile);
            _totalRecordsArray.resize(_allAmountProfile);

            // Create records
            for (int i = 0; i < _allProfiles.size(); ++i)
            {
                _allProfiles[i].historyRecord.reset(DLLStorage->createRecordsStream(ERecordTypes::HISTORY, i));
                _allProfiles[i].bookmarksRecord.reset(DLLStorage->createRecordsStream(ERecordTypes::BOOKMARKS, i));
                _allProfiles[i].loginsRecord.reset(DLLStorage->createRecordsStream(ERecordTypes::LOGINS, i));
                _allProfiles[i].cacheRecord.reset(DLLStorage->createRecordsStream(ERecordTypes::CACHEFILES, i));

                _totalRecordsArray[i].resize(4);
            }
        }
    }
    else
    {
        setMainFormEnabled(false);

        QMessageBox::warning(this, tr("Warning"),
                             tr("Couldn't load FFParser_DLL!\n"),
                             QMessageBox::Ok);
    }
}

void MainWindow::setMainFormEnabled(bool enabled)
{
    ui.centralWidget->setEnabled(enabled);
    ui.menuFile->actions().at(4)->setEnabled(enabled);
    ui.menuFile->actions().at(5)->setEnabled(enabled);
    ui.menuFile->actions().at(7)->setEnabled(enabled);
}

void MainWindow::createHelpMenu()
{
    QAction *aboutMenu = new QAction(tr("About"), ui.menuHelp);
    aboutMenu->setShortcut(QKeySequence(QKeySequence(Qt::Key_F1)));
    ui.menuHelp->addAction(aboutMenu);
    connect(aboutMenu, &QAction::triggered, this, &MainWindow::slotAboutMenu);
}

void MainWindow::slotAboutMenu()
{
    QMessageBox::about(this, tr("About FFParser"),
                       tr("Firefox Cache/History Parser\n"
                          "Version: 1.0\n"
                          "Developers: Igor Yarovoy, Dmitriy Voloshyn\n"
                          "©Apriorit, 2017"));
}

void MainWindow::createFileMenu()
{
    QAction *openFileMenu = new QAction(tr("Open selected file"), ui.menuFile);
    openFileMenu->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    openFileMenu->setDisabled(true);
    ui.menuFile->addAction(openFileMenu);
    connect(openFileMenu, &QAction::triggered, this, &MainWindow::slotOpenSelectedFileMenu);

    QAction *exportFile = new QAction(tr("Export selected file"), ui.menuFile);
    exportFile->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_E));
    exportFile->setDisabled(true);
    ui.menuFile->addAction(exportFile);
    connect(exportFile, &QAction::triggered, this, &MainWindow::slotExportSelectedFile);

    QAction *showInExplr = new QAction(tr("Show file in explorer"), ui.menuFile);
    showInExplr->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
    showInExplr->setDisabled(true);
    ui.menuFile->addAction(showInExplr);
    connect(showInExplr, &QAction::triggered, this, &MainWindow::slotShowInExplorer);

    ui.menuFile->addSeparator();

    QAction *openProfileFolder = new QAction(tr("Open profile folder"), ui.menuFile);
    ui.menuFile->addAction(openProfileFolder);
    connect(openProfileFolder, &QAction::triggered, this, &MainWindow::slotOpenProfileFolder);


    QAction *openCacheFolder = new QAction(tr("Open cache folder"), ui.menuFile);
    ui.menuFile->addAction(openCacheFolder);
    connect(openCacheFolder, &QAction::triggered, this, &MainWindow::slotOpenCacheFolder);

    ui.menuFile->addSeparator();

    QAction *exportMenu = new QAction(tr("Export"), ui.menuFile);
    exportMenu->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_P));
    ui.menuFile->addAction(exportMenu);
    connect(exportMenu, &QAction::triggered, this, &MainWindow::slotMenuExport);

    ui.menuFile->addSeparator();

    QAction *exitMenu = new QAction(tr("Exit"), ui.menuFile);
    exitMenu->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    ui.menuFile->addAction(exitMenu);
    connect(exitMenu, &QAction::triggered, this, &MainWindow::slotExitProgram);
}

void MainWindow::slotOpenSelectedFileMenu()
{
    this->exportAndOpenSelected();
}


void MainWindow::slotExportSelectedFile()
{
    ExportCacheFileDialog exportCache(this);
    exportCache.exec();
}

void MainWindow::slotShowInExplorer()
{
    this->showSelectedFileInExplorer();
}

void MainWindow::slotOpenProfileFolder()
{
    QString path = DLLStorage->getPathToProfile(_profileNumber);

    QStringList args;

    args << QDir::toNativeSeparators(path);

    QProcess *process = new QProcess(this);
    process->start("explorer.exe", args);
}

void MainWindow::slotOpenCacheFolder()
{
    QString path = DLLStorage->getPathToCache(_profileNumber);

    QStringList args;

    args << QDir::toNativeSeparators(path);

    QProcess *process = new QProcess(this);
    process->start("explorer.exe", args);
}

void MainWindow::slotMenuExport()
{
    _exportFileWindow->show();
}

void MainWindow::slotExitProgram()
{
    QMessageBox::StandardButton resBtn = QMessageBox::question(this, tr("Close"),
                                                               tr("Close FFParser?\n"),
                                                               QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                               QMessageBox::Yes);
    if (resBtn == QMessageBox::Yes)
    {
        QApplication::quit();
    }
}


ERecordTypes MainWindow::getCurrentTabType()
{
    switch (ui.tabWidget->currentIndex())
    {
        case 0: return ERecordTypes::HISTORY;
        case 1: return ERecordTypes::BOOKMARKS;
        case 2: return ERecordTypes::LOGINS;
        case 3: return ERecordTypes::CACHEFILES;
    }
}


QStringList MainWindow::getProfiles()
{
    QStringList profiles;
    for (int i = 0; i < ui.chooseProfile->count(); ++i) {
        profiles.push_back(ui.chooseProfile->itemText(i));
    }

    return profiles;
}

const recordPtr & MainWindow::getPtr(ERecordTypes type, size_t numberProfile)
{
    const ProfileRecordData  &profileData = _allProfiles[numberProfile];

    switch (type)
    {
    case ERecordTypes::HISTORY:
        return profileData.historyRecord;
    case ERecordTypes::BOOKMARKS:
        return profileData.bookmarksRecord;
    case ERecordTypes::LOGINS:
        return profileData.loginsRecord;
    case ERecordTypes::CACHEFILES:
        return profileData.cacheRecord;
    default:
        break;
    }
}

void MainWindow::exportSelectedFile(const char *path, bool md5)
{
    size_t row = ui.tableWidget->selectionModel()->currentIndex().row();

    const recordPtr &currPtr = getCurrentPtr();

    if (currPtr != nullptr)
    {
        IRecord *recordPtr = currPtr->getRecordByIndex(_firstRecord + row);
        IDataExporter *exporter = DLLStorage->getDataExporter();
        exporter->exportCacheFile(recordPtr, path, md5);
    }
}


void MainWindow::exportAndOpenSelected()
{
    //get file name
    QString fname = getTableField("filename");

    //get cache id
    QString cache_id = getTableField("path");
    int pos = cache_id.lastIndexOf("\\");
    cache_id.remove(0, pos+1);
    cache_id.insert(0, " (");
    cache_id += ")";

    //set path to file with id
    QString id_file = fname;
    pos = id_file.lastIndexOf(".");
    id_file.insert(pos, cache_id);

    //check if file with cache id exists
    QUrl path = QUrl::fromLocalFile(tempDirPath + id_file);
    if (!QDesktopServices::openUrl(path))
    {
        //try unique file
        QUrl temp = QUrl::fromLocalFile(tempDirPath + fname);
        if (!QDesktopServices::openUrl(temp))
        {
            //export selected file
            exportSelectedFile(tempDirPath.toStdString().c_str(), false);

            //open file
            if (!QDesktopServices::openUrl(temp)) {
                QMessageBox::warning(this, "Warning",
                                             tr("Cannot open file\n"),
                                             QMessageBox::Ok);
            }
        }
    }
}


void MainWindow::showSelectedFileInExplorer()
{
    QString path = this->getTableField("path");

    QStringList args;

    args << "/select," << QDir::toNativeSeparators(path);

    QProcess *process = new QProcess(this);
    process->start("explorer.exe", args);
}


QTableWidgetItem* MainWindow::getLastClickedItem()
{
    return _lastClickedItem;
}

const recordPtr & MainWindow::getCurrentPtr()
{
    switch (ui.tabWidget->currentIndex())
    {
    case 0:
        return getPtr(ERecordTypes::HISTORY, _profileNumber);
    case 1:
        return getPtr(ERecordTypes::BOOKMARKS, _profileNumber);
    case 2:
        return getPtr(ERecordTypes::LOGINS, _profileNumber);
    case 3:
        return getPtr(ERecordTypes::CACHEFILES, _profileNumber);
    default:
        break;
    }
}


size_t& MainWindow::getCurrentStep()
{
    return _stepForTabs[ui.tabWidget->currentIndex()];
}

size_t& MainWindow::getCurrentTotalRecords()
{
    return _totalRecordsArray[_profileNumber][ui.tabWidget->currentIndex()];
}

IDataExporter *MainWindow::getExporter()
{
    return DLLStorage->getDataExporter();
}

void MainWindow::setTableHeaders()
{
    const recordPtr &ptr = getCurrentPtr();

    int counter = 0;

    QStringList headers;
    while (ptr->getFieldName(counter) != nullptr)
    {
        headers += ptr->getFieldName(counter);
        ++counter;
    }

    //update table UI
    ui.tableWidget->setColumnCount(counter);
    ui.tableWidget->setHorizontalHeaderLabels(headers);

    for (int i = 0; i < counter; ++i) {
        ui.tableWidget->setColumnWidth(i, ui.tableWidget->width() / counter);
    }
}


void MainWindow::viewRecords()
{
    const recordPtr &ptr = getCurrentPtr();

    //clear previous rows
    ui.tableWidget->setRowCount(0);

    IRecord* onerec = ptr->getRecordByIndex(_firstRecord);

    if (onerec == nullptr) {
        viewCounterRecords(0, 0);
        return;
    }

    ptr->setCurrentRecord(_firstRecord);

    ui.progressBar->setMaximum(getCurrentStep());
    ui.progressBar->setValue(0);

    QStringList actualRowNumbers;

    size_t iterator = 0;
    size_t max_count = _firstRecord + getCurrentStep();

    //read records to table
    for (size_t i = _firstRecord; i < max_count; ++i)
    {
        ui.tableWidget->insertRow(iterator);

        //add actual row number
        actualRowNumbers.push_back(QString::number(i + 1));

        size_t counter = 0;
        while (onerec->getFieldValue(counter) != nullptr)
        {
            ui.tableWidget->setItem(iterator, counter, new QTableWidgetItem(onerec->getFieldValue(counter)));
            ++counter;
        }

        ++iterator;
        onerec = ptr->getNextRecord();

        size_t currValue = ui.progressBar->value();
        ui.progressBar->setValue(currValue + 1);


        if (onerec == nullptr)
            break;
    }

    viewCounterRecords(_firstRecord, _firstRecord + iterator);

    //set actual row numbers
    ui.tableWidget->setVerticalHeaderLabels(actualRowNumbers);

    //search text in updated set
    search();
}

void MainWindow::setNameProfile()
{
    ui.chooseProfile->clear();
    for (size_t counter = 0; counter < _allAmountProfile; ++counter)
        ui.chooseProfile->addItem(DLLStorage->getProfileName(counter));
}


void MainWindow::createTableUI()
{
    ui.tableWidget->setShowGrid(true);

    ui.tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui.tableWidget->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui.tableWidget, SIGNAL(clicked(QModelIndex)), this, SLOT(slotCloseContextMenu()));
    connect(ui.tableWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomMenuRequested(QPoint)));
}

void MainWindow::slotCustomMenuRequested(QPoint pos)
{
    _menu->popup(ui.tableWidget->viewport()->mapToGlobal(pos));
}

void MainWindow::slotCloseContextMenu()
{
    _menu->hide();
}

QString MainWindow::getTableField(const char *fieldName)
{
    size_t row = ui.tableWidget->selectionModel()->currentIndex().row();

    IRecord *recordPtr = getCurrentPtr()->getRecordByIndex(_firstRecord + row);
    const char *foundString = recordPtr->getFieldByName(fieldName);

    if (foundString != nullptr) {
        return QString(foundString);
    }

    return "";
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question(this, tr("Close"),
                                                               tr("Close FFParser?\n"),
                                                               QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                               QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes)
    {
        event->ignore();
    }
    else
    {
        event->accept();
    }
}


void MainWindow::createLanguageMenu(void)
{
    _changeLanguage->fillLanguageMenu(ui.menuLanguage);
}

// Called every time, when a menu entry of the language menu is called
void MainWindow::slotLanguageChanged(QAction* action)
{
    if(action != nullptr)
    {
        // load the language dependant on the action content
        _changeLanguage->loadLanguage(action->data().toString());
    }
}

void MainWindow::changeEvent(QEvent* event)
{
    if(event != nullptr)
    {
        switch(event->type())
        {
            // this event is send if a translator is loaded
            case QEvent::LanguageChange:
                ui.retranslateUi(this);
                break;

            // this event is send, if the system, language changes
            case QEvent::LocaleChange:
            {
                QString locale = QLocale::system().name();
                locale.truncate(locale.lastIndexOf('_'));
                _changeLanguage->loadLanguage(locale);
                break;
            }
            default:
                break;
        }
    }
    QMainWindow::changeEvent(event);
}


void MainWindow::loadNewRecords()
{
    const recordPtr &ptr = getCurrentPtr();

    size_t loaded = ptr->getNumberOfRecords();
    size_t& step = getCurrentStep();
    size_t& total = getCurrentTotalRecords();

    //if initial load
    if (ptr->getNumberOfRecords() == 0)
    {
        total = ptr->getTotalRecords(); //save total

        //set table headers
        setTableHeaders();
    }

    if (step > total) {
        step = total;
    }

    //load new if needed
    if (_firstRecord + step > loaded)
    {
        //load starts
        ui.progressBar->setValue(0);

        ptr->loadNextRecords(step);
    }

    //update step
    ui.spinBox->setValue(step);

    //update view
    viewRecords();
}


void MainWindow::on_setRecordButton_clicked()
{
    size_t& step = getCurrentStep();
    step = ui.spinBox->value();

    loadNewRecords();

    ui.spinBox->setValue(step);
}


void MainWindow::on_nextPageButton_clicked()
{
    size_t step = getCurrentStep();

    //check bounds
    if (_firstRecord + step < getCurrentTotalRecords())
    {
        _firstRecord += step;
        loadNewRecords();
    }
}


void MainWindow::on_prevPageButton_clicked()
{
    size_t curStep = getCurrentStep();

    //check bounds
    if (_firstRecord > curStep)
    {
        _firstRecord -= curStep;
        viewRecords();
    }
    else if (_firstRecord > 0) {
        _firstRecord = 0;
        viewRecords();
    }
}

void MainWindow::search()
{
    if (!_searchFlag) {
        return;
    }

    ui.clearSearchRecord->setEnabled(true);

    QString dataToFind = ui.lineEdit->text().toLower();
    if (dataToFind.size() >= 2)
    {
        size_t columnCount = ui.tableWidget->columnCount();
        size_t rowCount = ui.tableWidget->rowCount();

        if (columnCount != 0 && rowCount != 0)
        {
            size_t counterSearchRecords = 0;

            for (size_t i = 0; i < rowCount; ++i)
            {
                bool tempSearchFlag = false;
                for (size_t j = 0; j < columnCount; ++j)
                {
                    QTableWidgetItem *item =  ui.tableWidget->item(i, j);
                    QString temp = item->text().toLower();
                    size_t counter = 0;

                    while (temp.indexOf(dataToFind, counter) != -1)
                    {
                        ++counter;
                        size_t currCounter = 0;
                        while (currCounter < columnCount)
                        {
                            ui.tableWidget->item(i, currCounter)->setData(Qt::BackgroundRole, QColor (125, 237, 151));
                            ++currCounter;
                        }
                        tempSearchFlag = true;
                    }
                }
                if (tempSearchFlag)
                    ++counterSearchRecords;
            }
            ui.foundTextLabel->setText(tr("Found: ") + QString::number(counterSearchRecords));
            ui.foundTextLabel->show();
        }
        else
        {
            QMessageBox::warning(this, tr("Warning"),
                                 tr("The records are not loaded\n"),
                                 QMessageBox::Ok);
        }
    }
    else
    {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Search text's too short\n"),
                             QMessageBox::Ok);
    }
}


void MainWindow::on_searchButton_clicked()
{
    _searchFlag = true;
    search();
}


void MainWindow::on_tabWidget_currentChanged(int index)
{
    _firstRecord = 0;
    _searchFlag = false;
    ui.foundTextLabel->hide();

    loadNewRecords();
}

void MainWindow::viewCounterRecords(size_t first, size_t last)
{
    QString temp;

    if (first == 0 && last == 0 )
        temp = "0-0 / 0";
    else
        temp = QString::number(first + 1) + '-' + QString::number(last) + " / " + QString::number(getCurrentTotalRecords());

    ui.recordRangeTextLabel->setText(temp);
}

void MainWindow::on_clearSearchRecord_clicked()
{
    _searchFlag = false;
    ui.foundTextLabel->hide();

    size_t columnCount = ui.tableWidget->columnCount();
    size_t rowCount = ui.tableWidget->rowCount();

    for (size_t i = 0; i < rowCount; ++i)
    {
        for (size_t  j = 0; j < columnCount; ++j)
        {
            ui.tableWidget->item(i, j)->setData(Qt::BackgroundRole, QColor (255, 255, 255));
        }
    }

    ui.lineEdit->setText("");
    ui.clearSearchRecord->setEnabled(false);
}

void MainWindow::on_chooseProfile_activated(int index)
{
    _profileNumber = static_cast<size_t>(index);
    _firstRecord = 0;
    loadNewRecords();
}

void MainWindow::on_tableWidget_itemSelectionChanged()
{
    //update main menu
    if (getCurrentTabType() != ERecordTypes::CACHEFILES) {
        ui.menuFile->actions().at(0)->setDisabled(true);
        ui.menuFile->actions().at(1)->setDisabled(true);
        ui.menuFile->actions().at(2)->setDisabled(true);
    }
    else {
        ui.menuFile->actions().at(0)->setEnabled(true);
        ui.menuFile->actions().at(1)->setEnabled(true);
        ui.menuFile->actions().at(2)->setEnabled(true);
    }
}


void MainWindow::on_tableWidget_currentItemChanged(QTableWidgetItem *current, QTableWidgetItem *previous)
{
    _lastClickedItem = current;
}

void MainWindow::on_lineEdit_returnPressed()
{
    _searchFlag = true;
    search();
}
