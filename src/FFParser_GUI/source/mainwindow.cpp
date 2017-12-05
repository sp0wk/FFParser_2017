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

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    dllname(L"FFParser_DLL.dll"),
    tempDirPath(QCoreApplication::applicationDirPath() + "/temp/"),
    tempDir(tempDirPath),
    dll_load(LoadLibrary(dllname), [](HMODULE dll) { if (dll) FreeLibrary(dll); }),
    _exportFileWindow(new Export(this)),
    _menu(new ContextMenu(this)),
    _changeLanguage(new ChangeLanguage(this)),
    _firstRecord(0),
    _oldStep(25),
    _profileNumber(0),
    _allAmountProfile(0),
    _searchFlag(false)
{
    ui.setupUi(this);
    createLanguageMenu();
    createFileMenu();
    createHelpMenu();

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
            // Create records
            for (int i = 0; i < _allProfiles.size(); ++i)
            {
                _allProfiles[i].historyRecord.reset(DLLStorage->createRecordsStream(ERecordTypes::HISTORY, i));
                _allProfiles[i].bookmarksRecord.reset(DLLStorage->createRecordsStream(ERecordTypes::BOOKMARKS, i));
                _allProfiles[i].loginsRecord.reset(DLLStorage->createRecordsStream(ERecordTypes::LOGINS, i));
                _allProfiles[i].cacheRecord.reset(DLLStorage->createRecordsStream(ERecordTypes::CACHEFILES, i));
            }
            stepForTabs.resize(ui.tabWidget->count());
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

    const recordPtr &currPtr = getPtrByTabIndex(ui.tabWidget->currentIndex());

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

const recordPtr & MainWindow::getPtrByTabIndex(size_t index)
{
    switch (index)
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

IDataExporter *MainWindow::getExporter()
{
    return DLLStorage->getDataExporter();
}

void MainWindow::setNameColumnTable(const recordPtr &ptr)
{
    int counter = 0;

    QStringList temp;
    while (ptr->getFieldName(counter) != nullptr)
    {
        temp += ptr->getFieldName(counter);
        ++counter;
    }

    createUI(temp, counter);
}

void MainWindow::removeRowTable(size_t counter)
{
    if (counter == 0)
        return;

    ui.tableWidget->clearContents();

    while (counter != -1)
    {
        ui.tableWidget->removeRow(counter);
        --counter;
    }
}
void MainWindow::viewRecord(const recordPtr &ptr)
{
    removeRowTable(_oldStep);


    initialLoadRecord(ptr);

    IRecord* onerec = ptr->getRecordByIndex(_firstRecord);
    if (onerec == nullptr)
        return;

    ptr->setCurrentRecord(_firstRecord);

    size_t tempStep = stepForTabs[ui.tabWidget->currentIndex()];
    size_t total = ptr->getTotalRecords();

    if (tempStep > total) {
        stepForTabs[ui.tabWidget->currentIndex()] = total;
    }

    ui.progressBar->setRange(0, stepForTabs[ui.tabWidget->currentIndex()]);
    ui.progressBar->setValue(0);
    ui.progressBar->setAutoFillBackground(false);

    QStringList actualRowNumbers;

    size_t iterator = 0;
    size_t max_count = _firstRecord + stepForTabs[ui.tabWidget->currentIndex()];

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

    viewCounterRecords(_firstRecord, _firstRecord + iterator, ptr);
    _oldStep = stepForTabs[ui.tabWidget->currentIndex()];

    //set actual row numbers
    ui.tableWidget->setVerticalHeaderLabels(actualRowNumbers);

}

void MainWindow::setNameProfile()
{
    ui.chooseProfile->clear();
    for (size_t counter = 0; counter < _allAmountProfile; ++counter)
        ui.chooseProfile->addItem(DLLStorage->getProfileName(counter));
}


void MainWindow::initialLoadRecord(const recordPtr &ptr)
{
    if (ptr->getNumberOfRecords() == 0)
    {
        size_t tempStep = ui.spinBox->value();
        size_t total = ptr->getTotalRecords();

        if (tempStep > total)
            tempStep = total;
        ptr->loadRecords(0, tempStep);
        stepForTabs[ui.tabWidget->currentIndex()] = tempStep;
    }
}


void MainWindow::createUI(const QStringList &headers, size_t number)
{
    ui.tableWidget->setColumnCount(number);
    ui.tableWidget->setShowGrid(true);

    ui.tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    ui.tableWidget->setHorizontalHeaderLabels(headers);
    ui.tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    for (size_t i = 0; i < number; ++i)
        ui.tableWidget->setColumnWidth(i, ui.tableWidget->width() / number);

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

    const recordPtr &currPtr = getPtrByTabIndex(ui.tabWidget->currentIndex());

    IRecord *recordPtr = currPtr->getRecordByIndex(_firstRecord + row);
    const char *foundString = recordPtr->getFieldByName(fieldName);
    return QString(foundString);

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
    QActionGroup* langGroup = new QActionGroup(ui.menuLanguage);
    langGroup->setExclusive(true);

    connect(langGroup, SIGNAL (triggered(QAction *)), this, SLOT (slotLanguageChanged(QAction *)));

    // format systems language
    QString defaultLocale = QLocale::system().name(); // e.g. "ru_RU"
    defaultLocale.truncate(defaultLocale.lastIndexOf('_')); // e.g. "ru"

    QString langPath = QApplication::applicationDirPath();
    _changeLanguage->setLangPath(langPath.append("/languages"));
    QDir dir(langPath);
    QStringList fileNames = dir.entryList(QStringList("TranslationExample_*.qm"));

    for (int i = 0; i < fileNames.size(); ++i)
    {
      // get locale extracted by filename
        QString locale;
        locale = fileNames[i];      // "TranslationExample_ru.qm"
        locale.truncate(locale.lastIndexOf('.'));       // "TranslationExample_ru"
        locale.remove(0, locale.indexOf('_') + 1);      // "ru"

        QString lang = QLocale::languageToString(QLocale(locale).language());

        QAction *action = new QAction(lang, this);
        action->setCheckable(true);
        action->setData(locale);

        ui.menuLanguage->addAction(action);
        langGroup->addAction(action);

        // set default translators and language checked
        if (defaultLocale == locale)
        {
            action->setChecked(true);
        }
    }
}

// Called every time, when a menu entry of the language menu is called
void MainWindow::slotLanguageChanged(QAction* action)
{
    if(action != nullptr)
    {
        // load the language dependant on the action content
        _changeLanguage->loadLanguage(action->data().toString());
        //loadLanguage(action->data().toString());
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

void MainWindow::switchViewRecords(size_t index)
{
    const recordPtr &currPtr = getPtrByTabIndex(index);
    setNameColumnTable(currPtr);
    viewRecord(currPtr);
}

void MainWindow::checkNewRecords(size_t indexTab, size_t first, size_t step)
{
    const recordPtr &currPtr = getPtrByTabIndex(indexTab);
    size_t totalLoadRecords = currPtr->getNumberOfRecords();
    if (first + step >= totalLoadRecords)
    {
        size_t temp = (first + step) - totalLoadRecords;
        if (temp != 0)
            currPtr->loadNextRecords(temp);
        else
            currPtr->loadNextRecords(step);

    }
}

void MainWindow::on_setRecordButton_clicked()
{
    size_t tempStep = ui.spinBox->value();
    size_t tempIndex = ui.tabWidget->currentIndex();

    checkNewRecords(tempIndex, _firstRecord, tempStep);
    stepForTabs[tempIndex] = tempStep;
    ui.progressBar->setValue(0);
    switchViewRecords(tempIndex);
}

bool MainWindow::isOutOfRange(size_t indexTab, size_t first, size_t step)
{
    return (first + step >= getPtrByTabIndex(indexTab)->getTotalRecords());
}

void MainWindow::on_nextPageButton_clicked()
{
    checkNewRecords(ui.tabWidget->currentIndex(), _firstRecord, stepForTabs[ui.tabWidget->currentIndex()]);
    if (isOutOfRange(ui.tabWidget->currentIndex(), _firstRecord, stepForTabs[ui.tabWidget->currentIndex()]) == false)
    {
        _firstRecord += stepForTabs[ui.tabWidget->currentIndex()];
    }

    ui.progressBar->setValue(0);
    switchViewRecords(ui.tabWidget->currentIndex());

    if (_searchFlag == true)
        search();
}

void MainWindow::on_prevPageButton_clicked()
{
    if (static_cast<int>(_firstRecord - stepForTabs[ui.tabWidget->currentIndex()]) <= 0)
    {
        _firstRecord = 0;
    }
    else
    {
        _firstRecord -= stepForTabs[ui.tabWidget->currentIndex()];
    }

    ui.progressBar->setValue(0);
    switchViewRecords(ui.tabWidget->currentIndex());

    if (_searchFlag == true)
        search();
}

void MainWindow::search()
{
    ui.clearSearchRecord->setEnabled(true);
    _searchFlag = true;

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
    search();
}

void MainWindow::viewStep(size_t indexTab)
{
    ui.spinBox->setValue(stepForTabs[indexTab]);
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    _firstRecord = 0;

    if (_searchFlag)
        _searchFlag = false;

    if (ui.foundTextLabel->isVisible())
        ui.foundTextLabel->hide();

    ui.progressBar->setValue(0);
    switchViewRecords(static_cast<size_t>(index));
    viewStep(static_cast<size_t>(index));
}

void MainWindow::viewCounterRecords(size_t first, size_t last, const recordPtr &ptr)
{
    QString temp = QString::number(first + 1) + '-' + QString::number(last) + " / " + QString::number(ptr->getTotalRecords());
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
    switchViewRecords(ui.tabWidget->currentIndex());
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
