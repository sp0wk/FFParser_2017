#include "mainwindow.h"
#include <QDebug>
#include <QVariant>




MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    historyRecord(nullptr),
    bookmarksRecord(nullptr),
    loginRecord(0),
    cacheRecord(0),
    firstRecord(0),
    lastRecord(0),
    step(25),
    oldStep(step),
    flag(0),
    counterRecords(0),
    profileNumber(0),
    counterProfile(0)
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

    //get history example
    historyRecord = DLLStorage->createRecordsStream(ERecordTypes::HISTORY, profileNumber);
    bookmarksRecord = DLLStorage->createRecordsStream(ERecordTypes::BOOKMARKS, profileNumber);
    loginRecord = DLLStorage->createRecordsStream(ERecordTypes::LOGINS, profileNumber);
    cacheRecord = DLLStorage->createRecordsStream(ERecordTypes::CACHEFILES, profileNumber);

    counterProfile = DLLStorage->getNumberOfProfiles();
    setNameProfile();

}

void MainWindow::setNameColumnTable(IRecordsStream *ptr)
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
void MainWindow::veiwRecord(IRecordsStream *ptr)
{
    removeRowTable(oldStep);
    counterRecords = ptr->loadRecords(firstRecord, step);

    if (counterRecords == 0)
        return;
    else if (step > counterRecords)
        step = counterRecords;

    IRecord* onerec = ptr->getRecordByIndex(firstRecord);
    lastRecord = firstRecord + step;


    size_t iterator = 0;

    for (size_t i = firstRecord; i < lastRecord; ++i)
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
    oldStep = step;
}

void MainWindow::setNameProfile()
{
    size_t counter = 0;
    while (counter < counterProfile)
    {
        ui.comboBox->addItem(DLLStorage->getProfileName(counter), counter);
        ++counter;
    }

}

MainWindow::~MainWindow()
{
    //delete ui;
}



void MainWindow::createUI(const QStringList &headers, size_t number)
{
    // Specify the number of columns
    ui.tableWidget->setColumnCount(number);
    ui.tableWidget->setShowGrid(true); // Insert the grid

    // Let's select only one element
    ui.tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    // Let's select the line by line
    ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Set the column headers
    ui.tableWidget->setHorizontalHeaderLabels(headers);

    // Stretch the last column in all available space
    ui.tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui.tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

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

void MainWindow::switchVeiwRecords(size_t index)
{
    switch (index)
    {
    case 0:
        flag = 0;
        setNameColumnTable(historyRecord);
        veiwRecord(historyRecord);
        break;
    case 1:
        flag = 1;
        setNameColumnTable(bookmarksRecord);
        veiwRecord(bookmarksRecord);
        break;
    case 2:
        flag = 2;
        setNameColumnTable(loginRecord);
        veiwRecord(loginRecord);
        break;
    case 3:
        flag = 3;
        setNameColumnTable(cacheRecord);
        veiwRecord(cacheRecord);
        break;
    default:
        qDebug() << "Nituda!!!\n";
        break;
    }
}

void MainWindow::on_tabWidget_tabBarClicked(int index)
{
    switchVeiwRecords(static_cast<size_t>(index));
}

void MainWindow::on_pushButton_3_clicked()
{
    if (ui.textEdit->toPlainText().toInt() > 0)
    {
        step = ui.textEdit->toPlainText().toInt();
    }
}


void MainWindow::on_pushButton_clicked()
{
    firstRecord = lastRecord;
    switchVeiwRecords(flag);
}

void MainWindow::on_pushButton_2_clicked()
{
    if (static_cast<int>(firstRecord - step) <= 0)
    {
        firstRecord = 0;
        lastRecord = step;
    }
    else
    {
        firstRecord -= step;
        lastRecord -= step;
    }
   switchVeiwRecords(flag);
}

void MainWindow::on_pushButton_4_clicked()
{

    if (counterRecords != 0)
    {
        QString dataToFind = ui.textEdit_2->toPlainText();
        size_t columnCount = ui.tableWidget->columnCount();
        size_t rowCount = ui.tableWidget->rowCount();
        for (int i = 0; i < rowCount; ++i)
        {
            for (int  j = 0; j < columnCount; ++j)
            {
                ui.tableWidget->item(i, j)->setData(Qt::BackgroundRole, QColor (255, 255, 255));
            }
        }
        for (size_t i = 0; i < rowCount; ++i)
        {
            for (size_t j = 0; j < columnCount; ++j)
            {
                QTableWidgetItem *item =  ui.tableWidget->item(i, j);


                QString temp = item->text();


                size_t counter = 0;

                while ((counter = temp.indexOf(dataToFind, counter)) != -1)
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
        QMessageBox::StandardButton resBtn = QMessageBox::warning(this, "Warning",
                                                                   tr("The records are not load!\n"),
                                                                   QMessageBox::Ok);
}

void MainWindow::on_comboBox_activated(int index)
{
    profileNumber = static_cast<size_t>(index);
}
