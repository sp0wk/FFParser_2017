#include "export.h"
#include "ui_export.h"
#include "../include/IDataExporter.h"
#include <QFileDialog>


Export::Export(MainWindow *parent) :
    QDialog(parent),
    ui(new Ui::Export),
    _mainwindow(parent)
{
    ui->setupUi(this);
    ui->pathInputText->setText(QCoreApplication::applicationFilePath());
    this->setWindowTitle(tr("Data export"));
}

Export::~Export()
{
    delete ui;
}

void Export::exportData()
{
    IDataExporter *exportPtr = _mainwindow->getExporter();


    size_t profileNumber = ui->profilesToExport->currentIndex();
    QString path = ui->pathInputText->text();

    bool md5file = ui->calculateMD5CheckBox->isChecked();

    const size_t size = 5;
    bool exportCheck[size];

    exportCheck[0] = ui->historyCheckBox->isChecked();
    exportCheck[1] = ui->bookmarksCheckBox->isChecked();
    exportCheck[2] = ui->loginsCheckBox->isChecked();
    exportCheck[3] = ui->cacheCheckBox->isChecked();
    exportCheck[4] = ui->cacheFilesCheckBox->isChecked();

    size_t counter = 0;

    for (size_t i = 0; i < size; ++i)
        counter += exportCheck[i];

    if (counter != 0)
        ui->progressBar->setRange(0, counter);

    ui->progressBar->setValue(0);

    if (ui->historyCheckBox->isChecked())
    {
        // Export history
        auto ptr = _mainwindow->getPtr(ERecordTypes::HISTORY, profileNumber).data();
        ptr->loadNextRecords(); // Load all records
        exportPtr->exportRecords(ptr, path.toStdString().c_str(), md5file);
        size_t currValue = ui->progressBar->value();
        ui->progressBar->setValue(currValue + 1);
    }

    if (ui->bookmarksCheckBox->isChecked())
    {
        // Export bookmarks
        auto ptr = _mainwindow->getPtr(ERecordTypes::BOOKMARKS, profileNumber).data();
        ptr->loadNextRecords(); // Load all records
        exportPtr->exportRecords(ptr, path.toStdString().c_str(), md5file);
        size_t currValue = ui->progressBar->value();
        ui->progressBar->setValue(currValue + 1);
    }

    if (ui->loginsCheckBox->isChecked())
    {
        // todo something
        auto ptr = _mainwindow->getPtr(ERecordTypes::LOGINS, profileNumber).data();
        ptr->loadNextRecords(); // Load all records
        exportPtr->exportRecords(ptr, path.toStdString().c_str(), md5file);
        size_t currValue = ui->progressBar->value();
        ui->progressBar->setValue(currValue + 1);
    }

    if (ui->cacheCheckBox->isChecked())
    {
        // todo something
        auto ptr = _mainwindow->getPtr(ERecordTypes::CACHEFILES, profileNumber).data();
        ptr->loadNextRecords(); // Load all records
        exportPtr->exportRecords(ptr, path.toStdString().c_str(), md5file);
        size_t currValue = ui->progressBar->value();
        ui->progressBar->setValue(currValue + 1);
    }


    if (ui->cacheFilesCheckBox->isChecked())
    {
        // todo
        auto ptr = _mainwindow->getPtr(ERecordTypes::CACHEFILES, profileNumber).data();
        ptr->loadNextRecords(); // Load all record
        QString cachePath = path + "/cache_files";
        exportPtr->exportCache(ptr, cachePath.toStdString().c_str(), profileNumber, md5file);
        size_t currValue = ui->progressBar->value();
        ui->progressBar->setValue(currValue + 1);
    }
}


void Export::show()
{
    //set profiles
    ui->profilesToExport->clear();
    ui->profilesToExport->addItems(_mainwindow->getProfiles());

    ui->progressBar->setValue(0);
    ui->historyCheckBox->setChecked(false);
    ui->bookmarksCheckBox->setChecked(false);
    ui->loginsCheckBox->setChecked(false);
    ui->cacheCheckBox->setChecked(false);
    ui->cacheFilesCheckBox->setChecked(false);

    QDialog::show();
}

void Export::on_exportButton_clicked()
{
    exportData();
}

void Export::on_browseButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(0, ("Select Output Folder"), ui->pathInputText->text());
    if (path != "")
        ui->pathInputText->setText(path);
}
