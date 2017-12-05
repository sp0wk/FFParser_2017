#ifndef CHANGELANGUAGE_H
#define CHANGELANGUAGE_H

#include "mainwindow.h"
#include <QTranslator>
#include <QString>
#include <QLocale>

class ChangeLanguage
{
public:
    ChangeLanguage(MainWindow *parent);

    void fillLanguageMenu(QMenu* langMenu);
    void loadLanguage(const QString &rLanguage);
    void setLangPath(const QString &path);
    QString getLangPath() const;

private:
    MainWindow *_mainwindow;

    QTranslator _translator;
    QTranslator _translatorQt;
    QString _currLang;
    QString _langPath;

    void switchTranslator(QTranslator &translator, const QString &filename);

};

#endif // CHANGELANGUAGE_H
