#include "changelanguage.h"

ChangeLanguage::ChangeLanguage(MainWindow *parent)
    : _mainwindow(parent)
{

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
