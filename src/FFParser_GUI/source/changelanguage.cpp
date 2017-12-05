#include "changelanguage.h"

ChangeLanguage::ChangeLanguage(MainWindow *parent)
    : _mainwindow(parent)
{

}


void ChangeLanguage::fillLanguageMenu(QMenu* langMenu)
{
    QActionGroup* langGroup = new QActionGroup(langMenu);
    langGroup->setExclusive(true);

    QObject::connect(langGroup, SIGNAL (triggered(QAction *)), _mainwindow, SLOT (slotLanguageChanged(QAction *)));

    // format systems language
    QString defaultLocale = QLocale::system().name(); // e.g. "ru_RU"
    defaultLocale.truncate(defaultLocale.lastIndexOf('_')); // e.g. "ru"

    QString langPath = QApplication::applicationDirPath();
    setLangPath(langPath.append("/languages"));
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

        QAction *action = new QAction(langGroup);
        action->setText(lang);
        action->setCheckable(true);
        action->setData(locale);

        langMenu->addAction(action);
        langGroup->addAction(action);

        // set default translators and language checked
        if (defaultLocale == locale)
        {
            action->setChecked(true);
        }
    }
}


void ChangeLanguage::loadLanguage(const QString &rLanguage)
{
    if(_currLang != rLanguage)
    {
        _currLang = rLanguage;
        QLocale locale = QLocale(_currLang);
        QLocale::setDefault(locale);
        QString languageName = QLocale::languageToString(locale.language());
        switchTranslator(_translator, QString("languages/TranslationExample_%1.qm").arg(rLanguage));
        _mainwindow->statusBar()->showMessage("Current Language changed to " + languageName);
    }
}

void ChangeLanguage::switchTranslator(QTranslator &translator, const QString &filename)
{
    // remove the old translator
    qApp->removeTranslator(&translator);

    // load the new translator
    if(translator.load(filename))
        qApp->installTranslator(&translator);
}

void ChangeLanguage::setLangPath(const QString &path)
{
    _langPath = path;
}

QString ChangeLanguage::getLangPath() const
{
    return _langPath;
}
