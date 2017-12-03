#include "mainwindow.h"
#include <QDebug>
#include <QVariant>
#include "contextmenu.h"
#include "export.h"

using recordPtr = MainWindow::recordPtr;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    dllname(L"FFParser_DLL.dll"),
    tempDir(QCoreApplication::applicationFilePath() + "/temp"),
    dll_load(LoadLibrary(dllname), [](HMODULE dll) { if (dll) FreeLibrary(dll); }),
    _exportFileWindow(new Export(this)),
    _menu(new ContextMenu(this)),
    _firstRecord(0),
    oldStep(25),
    _profileNumber(0),
    _allAmountProfile(0),
    _searchFlag(false)
{
    ui.setupUi(this);
    createLanguageMenu();
    createFileMenu();

    if (dll_load.get() == nullptr) {
        QMessageBox::warning(this, "Warning",
                             tr("Couldn't load FFParser_DLL!\n"),
                             QMessageBox::Ok);
        exit(1);
    }

    GetStorageFunc dll_getstorage = (GetStorageFunc) GetProcAddress(dll_load.get(), "GetStorage");

    DLLStorage = dll_getstorage();


    _allAmountProfile = DLLStorage->getNumberOfProfiles();
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

void MainWindow::createFileMenu()
{
    QAction *exportMenu = new QAction(tr("Export"), ui.menuFile);
    ui.menuFile->addAction(exportMenu);
    connect(exportMenu, &QAction::triggered, this, &MainWindow::slotMenuExport);
}

void MainWindow::slotMenuExport()
{
    for (int i = 0; i < ui.chooseProfile->count(); ++i)
        _exportFileWindow->addProfileToCombobox(ui.chooseProfile->itemText(i));

    _exportFileWindow->show();
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
    if (ptr != nullptr)
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
    else
    {
        QMessageBox::warning(this, "Warning",
                             tr("Required entries were not found on your computer!\n"),
                             QMessageBox::Ok);
    }

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
    removeRowTable(oldStep);

    if (initialLoadRecord(ptr))
    {
        IRecord* onerec = ptr->getRecordByIndex(_firstRecord);
        if (onerec == nullptr)
            return;

        ptr->setCurrentRecord(_firstRecord);

        size_t tempStep = stepForTabs[ui.tabWidget->currentIndex()];
        size_t total = ptr->getTotalRecords();

        if (tempStep > total) {
            stepForTabs[ui.tabWidget->currentIndex()] = total;
        }

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

            if (onerec == nullptr)
                break;
        }

        viewCounterRecords(_firstRecord, _firstRecord + iterator, ptr);
        oldStep = stepForTabs[ui.tabWidget->currentIndex()];

        //set actual row numbers
        ui.tableWidget->setVerticalHeaderLabels(actualRowNumbers);
    }
}

void MainWindow::setNameProfile()
{
    for (size_t counter = 0; counter < _allAmountProfile; ++counter)
        ui.chooseProfile->addItem(DLLStorage->getProfileName(counter), counter);
}


bool MainWindow::initialLoadRecord(const recordPtr &ptr)
{
    if (ptr != nullptr)
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
        return true;
    }
    else
    {
        QMessageBox::warning(this, "Warning",
                             tr("Required entries were not found on your computer!\n"),
                             QMessageBox::Ok);

        return false;
    }
}

MainWindow::~MainWindow()
{
    delete _exportFileWindow;
    delete _menu;
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
    connect(ui.chooseProfile, SIGNAL(currentIndexChanged(int)), this, SLOT(on_chooseProfile_currentIndexChanged(int)));
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

    if (currPtr != nullptr)
    {
        IRecord *recordPtr = currPtr->getRecordByIndex(row);
        const char *foundString = recordPtr->getFieldByName(fieldName);
        return QString(foundString);
    }

    return "";
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QMessageBox::StandardButton resBtn = QMessageBox::question(this, "Close",
                                                               tr("Do you want to close FFParser?\n"),
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

    m_langPath = QApplication::applicationDirPath();
    m_langPath.append("/languages");
    QDir dir(m_langPath);
    QStringList fileNames = dir.entryList(QStringList("TranslationExample_*.qm"));

    for (int i = 0; i < fileNames.size(); ++i)
    {
      // get locale extracted by filename
        QString locale;
        locale = fileNames[i];      // "TranslationExample_ru.qm"
        locale.truncate(locale.lastIndexOf('.'));       // "TranslationExample_ru"
        locale.remove(0, locale.indexOf('_') + 1);      // "ru"

        QString lang = QLocale::languageToString(QLocale(locale).language());
        QIcon ico(QString("%1/%2.png").arg(m_langPath).arg(locale));

        QAction *action = new QAction(ico, lang, this);
        action->setCheckable(true);
        action->setData(locale);

        ui.menuLanguage->addAction(action);
        langGroup->addAction(action);

        // set default translators and language checked
        if (defaultLocale == locale)
        {
            action->setChecked(true);
        }
        setWindowIcon(action->icon());
    }
}

// Called every time, when a menu entry of the language menu is called
void MainWindow::slotLanguageChanged(QAction* action)
{
    if(action != nullptr)
    {
        // load the language dependant on the action content
        loadLanguage(action->data().toString());
        setWindowIcon(action->icon());
    }
}

void switchTranslator(QTranslator& translator, const QString& filename)
{
    // remove the old translator
    qApp->removeTranslator(&translator);

    // load the new translator
    if(translator.load(filename))
        qApp->installTranslator(&translator);
}

void MainWindow::loadLanguage(const QString& rLanguage)
{
    if(m_currLang != rLanguage)
    {
        m_currLang = rLanguage;
        QLocale locale = QLocale(m_currLang);
        QLocale::setDefault(locale);
        QString languageName = QLocale::languageToString(locale.language());
        switchTranslator(m_translator, QString("languages/TranslationExample_%1.qm").arg(rLanguage));
        ui.statusBar->showMessage(tr("Current Language changed to %1").arg(languageName));
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
                loadLanguage(locale);
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
    if (getPtrByTabIndex(ui.tabWidget->currentIndex()) != nullptr)
    {
        size_t tempStep = ui.spinBox->value();
        size_t tempIndex = ui.tabWidget->currentIndex();

        checkNewRecords(tempIndex, _firstRecord, tempStep);
        stepForTabs[tempIndex] = tempStep;
        switchViewRecords(tempIndex);
    }
    else
    {
            QMessageBox::warning(this, "Warning",
                                 tr("Required entries were not found on your computer!\n"),
                                 QMessageBox::Ok);
    }
}

bool MainWindow::isOutOfRange(size_t indexTab, size_t first, size_t step)
{
    return (first + step >= getPtrByTabIndex(indexTab)->getTotalRecords());
}

void MainWindow::on_nextPageButton_clicked()
{
    if (getPtrByTabIndex(ui.tabWidget->currentIndex()) != nullptr)
    {
        checkNewRecords(ui.tabWidget->currentIndex(), _firstRecord, stepForTabs[ui.tabWidget->currentIndex()]);
        if (isOutOfRange(ui.tabWidget->currentIndex(), _firstRecord, stepForTabs[ui.tabWidget->currentIndex()]) == false)
        {
            _firstRecord += stepForTabs[ui.tabWidget->currentIndex()];
        }
        switchViewRecords(ui.tabWidget->currentIndex());
        if (_searchFlag == true)
            search();
    }
}

void MainWindow::on_prevPageButton_clicked()
{
    if (getPtrByTabIndex(ui.tabWidget->currentIndex()) != nullptr)
    {
        if (static_cast<int>(_firstRecord - stepForTabs[ui.tabWidget->currentIndex()]) <= 0)
        {
            _firstRecord = 0;
        }
        else
        {
            _firstRecord -= stepForTabs[ui.tabWidget->currentIndex()];
        }
       switchViewRecords(ui.tabWidget->currentIndex());
       if (_searchFlag == true)
               search();
    }
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
                                 tr("The records are not load!\n"),
                                 QMessageBox::Ok);
        }
    }
    else
    {
        QMessageBox::warning(this, tr("Warning"),
                             tr("Input value very small!\n"),
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

void MainWindow::on_chooseProfile_currentIndexChanged(int index)
{
    _profileNumber = static_cast<size_t>(index);
    _firstRecord = 0;
    switchViewRecords(ui.tabWidget->currentIndex());
}
