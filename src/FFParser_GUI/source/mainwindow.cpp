#include "mainwindow.h"
#include <QDebug>
#include <QVariant>


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    historyRecord(nullptr),
    bookmarksRecord(nullptr),
    loginRecord(0),
    cacheRecord(0),
    _firstRecord(0),
    oldStep(25),
    _profileNumber(0),
    _allAmountProfile(0),
    _searchFlag(false)
{
    ui.setupUi(this);
    createLanguageMenu();
    createFileMenu();


    //dll load
    dllname = L"FFParser_DLL.dll";

    dll_load = LoadLibrary(dllname);

    if (!dll_load) {
        exit(1);
    }

    GetStorageFunc dll_getstorage = (GetStorageFunc) GetProcAddress(dll_load, "GetStorage");

    DLLStorage = dll_getstorage();

    _allAmountProfile = DLLStorage->getNumberOfProfiles();
    setNameProfile();

    //get history example
    historyRecord = DLLStorage->createRecordsStream(ERecordTypes::HISTORY, _profileNumber);
    bookmarksRecord = DLLStorage->createRecordsStream(ERecordTypes::BOOKMARKS, _profileNumber);
    loginRecord = DLLStorage->createRecordsStream(ERecordTypes::LOGINS, _profileNumber);
    cacheRecord = DLLStorage->createRecordsStream(ERecordTypes::CACHEFILES, _profileNumber);

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
    exportFileWindow = new Export(this);

    for (size_t i = 0; i < _allAmountProfile; ++i)
        exportFileWindow->setProfile(DLLStorage->getProfileName(i));
    exportFileWindow->setProfileCombobox();
    exportFileWindow->show();
}

IRecordsStream *MainWindow::getPtr(const size_t &index)
{
    switch (index)
    {
    case 0:
        return historyRecord;
    case 1:
        return bookmarksRecord;
    case 2:
        return loginRecord;
    case 3:
        return cacheRecord;
    default:
        return nullptr;
    }
}


bool MainWindow::ptrIsNotNull(const size_t &index)
{
    IRecordsStream *tempPtr = getPtr(index);

    if (tempPtr != nullptr)
        return true;

    return false;
}

void MainWindow::setNameColumnTable(IRecordsStream *ptr)
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
void MainWindow::viewRecord(IRecordsStream *ptr)
{
    removeRowTable(oldStep);

    if (initialLoadRecord(ptr))
    {
        IRecord* onerec = ptr->getRecordByIndex(_firstRecord);
        if (onerec == nullptr)
            return;

        ptr->setCurrentRecord(_firstRecord);

        size_t iterator = 0;

        size_t tempStep = stepForTabs[ui.tabWidget->currentIndex()];
        size_t total = ptr->getTotalRecords();
        if (tempStep > total)
            stepForTabs[ui.tabWidget->currentIndex()] = total;

        for (size_t i = _firstRecord; i < _firstRecord + stepForTabs[ui.tabWidget->currentIndex()]; ++i)
        {
            ui.tableWidget->insertRow(iterator);
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
    }
}

void MainWindow::setNameProfile()
{
    for (size_t counter = 0; counter < _allAmountProfile; ++counter)
        ui.chooseProfile->addItem(DLLStorage->getProfileName(counter), counter);
}


bool MainWindow::initialLoadRecord(IRecordsStream *ptr)
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
    FreeLibrary(dll_load);
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

    connect(ui.tableWidget, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(slotCustomMenuRequested(QPoint)));
}

void MainWindow::slotCustomMenuRequested(QPoint pos)
{
    QMenu *menu = new QMenu(this);

    QAction *openFile = new QAction(trUtf8("Open File"), this);
    QAction *openUrl = new QAction(trUtf8("Open URL"), this);
    QAction *exports = new QAction(trUtf8("Export"), this);

    connect(openFile, SIGNAL(triggered(bool)), this, SLOT(slotOpenFile()));
    connect(openUrl, SIGNAL(triggered(bool)), this, SLOT(slotOpenUrl()));
    connect(exports, SIGNAL(triggered(bool)), this, SLOT(slotExport()));

    menu->addAction(openFile);
    menu->addAction(openUrl);
    menu->addAction(exports);

    menu->popup(ui.tableWidget->viewport()->mapToGlobal(pos));
}

const char *MainWindow::getColumnTableName(IRecordsStream *ptr, const char *string, const size_t &row)
{
    if (ptr != nullptr)
    {
        IRecord *recordPtr = ptr->getRecordByIndex(row);
        const char *foundString = recordPtr->getFieldByName(string);
        return foundString;
    }

    return nullptr;
}
void MainWindow::slotOpenFile()
{
    size_t row = ui.tableWidget->selectionModel()->currentIndex().row();

    IRecordsStream *currPtr = getPtr(ui.tabWidget->currentIndex());

    if (currPtr != nullptr)
    {
        const char *found = getColumnTableName(currPtr, "path", row);


        QUrl temp = QUrl::fromLocalFile(found);

        if (!QDesktopServices::openUrl(temp))
        {
            QMessageBox::warning(this, "Warning",
                                 tr("This is not path file!\n"),
                                 QMessageBox::Ok);
        }
    }
}

void MainWindow::slotOpenUrl()
{
    size_t row = ui.tableWidget->selectionModel()->currentIndex().row();

    IRecordsStream *currPtr = getPtr(ui.tabWidget->currentIndex());

    if (currPtr != nullptr)
    {

        const char *found = getColumnTableName(currPtr, "url", row);
        if (found == nullptr)
            found = getColumnTableName(currPtr, "hostname", row);


        QUrl temp = QString(found);

        if (!QDesktopServices::openUrl(temp))
        {
            QMessageBox::warning(this, "Warning",
                                 tr("This is not URL address!\n"),
                                 QMessageBox::Ok);
        }
    }
}

void MainWindow::slotExport()
{
    qDebug() << "row: " << ui.tableWidget->selectionModel()->currentIndex().row();
    qDebug() << "column: " << ui.tableWidget->selectionModel()->currentIndex().column();
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
    IRecordsStream *currPtr = getPtr(index);
    setNameColumnTable(currPtr);
    viewRecord(currPtr);
}

void MainWindow::checkNewRecords(const size_t &indexTab, const size_t &first, const size_t &step)
{
    IRecordsStream *currPtr = getPtr(indexTab);
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
    if (ptrIsNotNull(ui.tabWidget->currentIndex()))
    {
        size_t tempStep = ui.spinBox->value();
        size_t tempIndex = ui.tabWidget->currentIndex();

        checkNewRecords(tempIndex, 0, tempStep);
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

bool MainWindow::isOutOfRange(const size_t &indexTab, const size_t &first, const size_t &step)
{
    return (first + step >= getPtr(indexTab)->getTotalRecords());
}

void MainWindow::on_nextPageButton_clicked()
{
    if (ptrIsNotNull(ui.tabWidget->currentIndex()))
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
    if (ptrIsNotNull(ui.tabWidget->currentIndex()))
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

bool MainWindow::checkRecords(const size_t &indexTab)
{
    bool flag = false;
    IRecordsStream *currPtr = getPtr(indexTab);
    if (currPtr->getNumberOfRecords() != 0)
    {
        flag = true;
    }

    return flag;
}

void MainWindow::search()
{
    ui.clearSearchRecord->setEnabled(true);
    _searchFlag = true;

    QString dataToFind = ui.lineEdit->text().toLower();
    if (dataToFind.size() >= 2)
    {
        if (checkRecords(ui.tabWidget->currentIndex()))
        {
            size_t columnCount = ui.tableWidget->columnCount();
            size_t rowCount = ui.tableWidget->rowCount();
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
            ui.label_4->setText("Found: " + QString::number(counterSearchRecords));
            ui.label_4->show();
        }
        else
        {
            QMessageBox::warning(this, "Warning",
                                 tr("The records are not load!\n"),
                                 QMessageBox::Ok);
        }
    }
    else
    {
        QMessageBox::warning(this, "Warning",
                             tr("Input value very small!\n"),
                             QMessageBox::Ok);
    }
}


void MainWindow::on_searchButton_clicked()
{
    search();
}

void MainWindow::on_chooseProfile_activated(int index)
{
    _profileNumber = static_cast<size_t>(index);

    historyRecord = DLLStorage->createRecordsStream(ERecordTypes::HISTORY, _profileNumber);
    bookmarksRecord = DLLStorage->createRecordsStream(ERecordTypes::BOOKMARKS, _profileNumber);
    loginRecord = DLLStorage->createRecordsStream(ERecordTypes::LOGINS, _profileNumber);
    cacheRecord = DLLStorage->createRecordsStream(ERecordTypes::CACHEFILES, _profileNumber);

    _firstRecord = 0;
    switchViewRecords(ui.tabWidget->currentIndex());
}

void MainWindow::viewStep(const size_t &indexTab)
{
    ui.spinBox->setValue(stepForTabs[indexTab]);
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    _firstRecord = 0;
    if (_searchFlag)
        _searchFlag = false;
    switchViewRecords(static_cast<size_t>(index));
    viewStep(static_cast<size_t>(index));
}

void MainWindow::viewCounterRecords(const size_t &first, const size_t &last, IRecordsStream *ptr)
{
    QString temp = QString::number(first + 1) + '-' + QString::number(last) + " / " + QString::number(ptr->getTotalRecords());
    ui.label_3->setText(temp);
}

void MainWindow::on_clearSearchRecord_clicked()
{
    _searchFlag = false;
    ui.label_4->hide();

    size_t columnCount = ui.tableWidget->columnCount();
    size_t rowCount = ui.tableWidget->rowCount();

    for (int i = 0; i < rowCount; ++i)
    {
        for (int  j = 0; j < columnCount; ++j)
        {
            ui.tableWidget->item(i, j)->setData(Qt::BackgroundRole, QColor (255, 255, 255));
        }
    }

    ui.lineEdit->setText("");
    ui.clearSearchRecord->setEnabled(false);
}
