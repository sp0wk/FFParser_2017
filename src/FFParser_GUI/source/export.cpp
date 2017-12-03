#include "export.h"
#include "ui_export.h"
#include "../include/IDataExporter.h"

Export::Export(MainWindow *parent) :
    QDialog(parent),
    ui(new Ui::Export),
    _mainwindow(parent)
{
    ui->setupUi(this);
}

Export::~Export()
{
    delete ui;
}

void Export::setProfile(const QString &nameProfile)
{
    _profiles.push_back(nameProfile);
}

void Export::addProfileCombobox()
{
    for (size_t i = 0; i < _profiles.size(); ++i)
        ui->chooseProfile->addItem(_profiles[i], i);
}

void Export::exportData()
{
    // Считываем для какого профиля будем делать экспорт
    // Узнать, что будем экспортировать по чекбоксам
    // Вызов соответствующих методов

    size_t profileNumber = ui->chooseProfile->currentIndex();

    if (ui->historyCheckBox->isChecked())
    {
        // todo something
    }

    if (ui->bookmarksCheckBox->isChecked())
    {
        // todo something
    }

    if (ui->loginsCheckBox->isChecked())
    {
        // todo something
    }

    if (ui->cacheCheckBox->isChecked())
    {
        // todo something
    }


    if (ui->cacheFilesCheckBox->isChecked())
    {
        // todo
    }
}

