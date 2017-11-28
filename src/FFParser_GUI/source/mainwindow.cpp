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
    profileNumber(0),
    m_allAmountProfile(0),
    _searchFlag(false)
{
    ui.setupUi(this);
    createLanguageMenu();


    //dll load
    dllname = L"FFParser_DLL.dll";

    dll_load = LoadLibrary(dllname);

    if (!dll_load) {
        exit(1);
    }

    GetStorageFunc dll_getstorage = (GetStorageFunc) GetProcAddress(dll_load, "GetStorage");

    DLLStorage = dll_getstorage();

    m_allAmountProfile = DLLStorage->getNumberOfProfiles();
    setNameProfile();

    //get history example
    historyRecord = DLLStorage->createRecordsStream(ERecordTypes::HISTORY, profileNumber);
    bookmarksRecord = DLLStorage->createRecordsStream(ERecordTypes::BOOKMARKS, profileNumber);
    loginRecord = DLLStorage->createRecordsStream(ERecordTypes::LOGINS, profileNumber);
    cacheRecord = DLLStorage->createRecordsStream(ERecordTypes::CACHEFILES, profileNumber);


    for (int i = 0; i < ui.tabWidget->count(); ++i)
        stepForTabs.push_back(0);
}

bool MainWindow::ptrIsNotNull(const size_t &index)
{
    bool flag = false;

    switch (index)
    {
    case 0:
        if (historyRecord != nullptr)
        {
            flag = true;
        }
        break;
    case 1:
        if (bookmarksRecord != nullptr)
        {
            flag = true;
        }
        break;
    case 2:
        if (loginRecord != nullptr)
        {
            flag = true;
        }
        break;
    case 3:
        if (cacheRecord != nullptr)
        {
            flag = true;
        }
        break;
    default:
        break;
    }

    return flag;
}

void MainWindow::setNameColumnTable(IRecordsStream *ptr)
{
    if (ptrIsNotNull(ui.tabWidget->currentIndex()))
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
        QMessageBox::StandardButton resBtn = QMessageBox::warning(this, "Warning",
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
                QTableWidgetItem *item = new QTableWidgetItem(onerec->getFieldValue(counter));
                item->setFlags(item->flags() ^ Qt::ItemIsEditable);
                ui.tableWidget->setItem(iterator, counter, item);
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
    size_t counter = 0;
    while (counter < m_allAmountProfile)
    {
        ui.comboBox->addItem(DLLStorage->getProfileName(counter), counter);
        ++counter;
    }

}


bool MainWindow::initialLoadRecord(IRecordsStream *ptr)
{
    if (ptrIsNotNull(ui.tabWidget->currentIndex()))
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
        QMessageBox::StandardButton resBtn = QMessageBox::warning(this, "Warning",
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

    for (size_t i = 0; i < number; ++i)
    {
        ui.tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
    }
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
    if(0 != action)
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
    switch (index)
    {
    case 0:
        setNameColumnTable(historyRecord);
        viewRecord(historyRecord);
        break;
    case 1:
        setNameColumnTable(bookmarksRecord);
        viewRecord(bookmarksRecord);
        break;
    case 2:
        setNameColumnTable(loginRecord);
        viewRecord(loginRecord);
        break;
    case 3:
        setNameColumnTable(cacheRecord);
        viewRecord(cacheRecord);
        break;
    default:
        qDebug() << "Out of range!\n";
        break;
    }
}

void MainWindow::loadNewNextRecords(const size_t &indexTab, const size_t &counterElement)
{
    switch (indexTab) {

    case 0:
        historyRecord->loadNextRecords(counterElement);
        break;
    case 1:
        bookmarksRecord->loadNextRecords(counterElement);
        break;
    case 2:
        loginRecord->loadNextRecords(counterElement);
        break;
    case 3:
        cacheRecord->loadNextRecords(counterElement);
        break;
    default:
        break;
    }
}

void MainWindow::checkNewRecords(const size_t &indexTab, const size_t &first, const size_t &step)
{
    switch (indexTab)
    {
    case 0:
        if (first + step >= historyRecord->getNumberOfRecords())
        {
            size_t temp = (first + step) - historyRecord->getNumberOfRecords();
            loadNewNextRecords(indexTab, temp);
        }
        break;
    case 1:
        if (first + step >= bookmarksRecord->getNumberOfRecords())
        {
            size_t temp = (first + step) - bookmarksRecord->getNumberOfRecords();
            loadNewNextRecords(indexTab, temp);
        }
        break;
    case 2:
        if (first + step >= loginRecord->getNumberOfRecords())
        {
            size_t temp = (first + step) - loginRecord->getNumberOfRecords();
            loadNewNextRecords(indexTab, temp);
        }
        break;
    case 3:
        if (first + step >= cacheRecord->getNumberOfRecords())
        {
            size_t temp = (first + step) - cacheRecord->getNumberOfRecords();
            loadNewNextRecords(indexTab, temp);
        }
        break;
    default:
        break;
    }
}

void MainWindow::on_pushButton_3_clicked()
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
            QMessageBox::StandardButton resBtn = QMessageBox::warning(this, "Warning",
                                                                       tr("Required entries were not found on your computer!\n"),
                                                                       QMessageBox::Ok);
    }
}

bool MainWindow::isOutOfRange(const size_t &indexTab, const size_t &first, const size_t &step)
{
    bool flag = false;

    switch (indexTab)
    {
    case 0:
        if (first + step >= historyRecord->getTotalRecords())
        {
            flag = true;
        }
        break;
    case 1:
        if (first + step >= bookmarksRecord->getTotalRecords())
        {
            flag = true;
        }
        break;
    case 2:
        if (first + step >= loginRecord->getTotalRecords())
        {
            flag = true;
        }
        break;
    case 3:
        if (first + step >= cacheRecord->getTotalRecords())
        {
            flag = true;
        }
        break;
    default:
        break;
    }

    return flag;
}

void MainWindow::on_pushButton_clicked()
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

void MainWindow::on_pushButton_2_clicked()
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

    switch (indexTab)
    {
    case 0:
        if (historyRecord->getNumberOfRecords() != 0)
        {
            flag = true;
        }
        break;
    case 1:
        if (bookmarksRecord->getNumberOfRecords() != 0)
        {
            flag = true;
        }
        break;
    case 2:
        if (loginRecord->getNumberOfRecords() != 0)
        {
            flag = true;
        }
        break;
    case 3:
        if (cacheRecord->getNumberOfRecords() != 0)
        {
            flag = true;
        }
        break;
    default:
        break;
    }

    return flag;
}

void MainWindow::search()
{
    ui.pushButton_5->setEnabled(true);

    QString dataToFind = ui.lineEdit->text().toLower();
    if (dataToFind.size() >= 2)
    {
        if (checkRecords(ui.tabWidget->currentIndex()))
        {
            size_t columnCount = ui.tableWidget->columnCount();
            size_t rowCount = ui.tableWidget->rowCount();

            for (size_t i = 0; i < rowCount; ++i)
            {
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
                            ui.tableWidget->item(i, currCounter)->setData(Qt::BackgroundRole, QColor (250,0,0));
                            ++currCounter;
                        }
                    }

                }
            }
        }
        else
        {
            QMessageBox::StandardButton resBtn = QMessageBox::warning(this, "Warning",
                                                                   tr("The records are not load!\n"),
                                                                   QMessageBox::Ok);
        }
    }
    else
    {
        QMessageBox::StandardButton resBtn = QMessageBox::warning(this, "Warning",
                                                                   tr("Input value very small!\n"),
                                                                   QMessageBox::Ok);
    }
}


void MainWindow::on_pushButton_4_clicked()
{
    search();
    _searchFlag = true;
}

void MainWindow::on_comboBox_activated(int index)
{
    profileNumber = static_cast<size_t>(index);

    historyRecord = DLLStorage->createRecordsStream(ERecordTypes::HISTORY, profileNumber);
    bookmarksRecord = DLLStorage->createRecordsStream(ERecordTypes::BOOKMARKS, profileNumber);
    loginRecord = DLLStorage->createRecordsStream(ERecordTypes::LOGINS, profileNumber);
    cacheRecord = DLLStorage->createRecordsStream(ERecordTypes::CACHEFILES, profileNumber);

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
    switchViewRecords(static_cast<size_t>(index));
    viewStep(static_cast<size_t>(index));
}

void MainWindow::viewCounterRecords(const size_t &first, const size_t &last, IRecordsStream *ptr)
{
    QString temp = "Records: " + QString::number(first) + '-' + QString::number(last) + '/' + QString::number(ptr->getTotalRecords());
    ui.label_3->setText(temp);
}

void MainWindow::on_pushButton_5_clicked()
{
    _searchFlag = false;

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
    ui.pushButton_5->setEnabled(false);
}
