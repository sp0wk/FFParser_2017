#include "mainwindow.h"




MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)/*,
    ui(new Ui::MainWindow)*/
{
    ui.setupUi(this);
    createLanguageMenu();

     // Customize the appearance of the table
     this->createUI(QStringList() << tr("size")
                    << trUtf8("creation time")
                    << trUtf8("last modification time")
                    << trUtf8("last access time")
                    << trUtf8("URL"));

}

MainWindow::~MainWindow()
{
    //delete ui;
}



void MainWindow::createUI(const QStringList &headers)
{
    // Specify the number of columns
    ui.tableWidget->setColumnCount(5);
    ui.tableWidget->setShowGrid(true); // Insert the grid

    // Let's select only one element
    ui.tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    // Let's select the line by line
    ui.tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Set the column headers
    ui.tableWidget->setHorizontalHeaderLabels(headers);

    // Stretch the last column in all available space
    ui.tableWidget->horizontalHeader()->setStretchLastSection(true);


    // Column resizing by content
    ui.tableWidget->resizeColumnsToContents();
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

