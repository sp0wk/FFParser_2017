#include "export.h"
#include "ui_export.h"
#include "../include/IDataExporter.h"
#include <QFileDialog>
#include <QtConcurrent/QtConcurrent>

using SetStopParsingFunc = void (CALL *)(bool flag);
extern SetStopParsingFunc dll_stopParsing;


Export::Export(MainWindow *parent) :
    QDialog(parent),
    ui(new Ui::Export),
    _mainwindow(parent)
{
    ui->setupUi(this);
    ui->pathInputText->setText(QCoreApplication::applicationDirPath());
    this->setWindowTitle(tr("Data export"));
    this->setModal(true);

    connect(this, &Export::streamExportFinished, this, &Export::onExportFinishedSlot);
}

Export::~Export()
{
    delete ui;
}


void Export::s_exportRecords(Export* exp, IRecordsStream* rstream, const QString& path, bool md5)
{
    rstream->loadNextRecords(); // Load all records
    exp->exportPtr->exportRecords(rstream, path.toStdString().c_str(), md5);
    emit exp->streamExportFinished();
}


void Export::s_exportCache(Export* exp, IRecordsStream* rstream, const QString& path, size_t profile, bool md5)
{
    rstream->loadNextRecords(); //load all cache records

    //cache records
    if (exp->ui->cacheCheckBox->isChecked())
    {
        exp->exportPtr->exportRecords(rstream, path.toStdString().c_str(), md5);
        emit exp->streamExportFinished();
    }

    //cache files
    if (exp->ui->cacheFilesCheckBox->isChecked())
    {
        exp->exportPtr->exportCache(rstream, path.toStdString().c_str(), profile, md5);
        emit exp->streamExportFinished();
    }


}


void Export::onExportFinishedSlot()
{
    size_t currValue = ui->progressBar->value();
    ui->progressBar->setValue(currValue + 1);

    //if all finished
    if (ui->progressBar->value() <= ui->progressBar->maximum()) {
        ui->exportButton->setEnabled(true);
        _watchers.clear();
    }
}

void Export::exportData()
{
    ui->exportButton->setDisabled(true);
    exportPtr = _mainwindow->getExporter();
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
        QFutureWatcher<void>* watcher = _mainwindow->getWatcher(profileNumber, ERecordTypes::HISTORY);
        watcher->waitForFinished();
        QFuture<void> fut = QtConcurrent::run(s_exportRecords, this, ptr, path, md5file);
        watcher->setFuture(fut);
        _watchers.push_back(watcher);
    }

    if (ui->bookmarksCheckBox->isChecked())
    {
        // Export bookmarks
        auto ptr = _mainwindow->getPtr(ERecordTypes::BOOKMARKS, profileNumber).data();
        QFutureWatcher<void>* watcher = _mainwindow->getWatcher(profileNumber, ERecordTypes::BOOKMARKS);
        watcher->waitForFinished();
        QFuture<void> fut = QtConcurrent::run(s_exportRecords, this, ptr, path, md5file);
        watcher->setFuture(fut);
        _watchers.push_back(watcher);
    }

    if (ui->loginsCheckBox->isChecked())
    {
        // Export logins
        auto ptr = _mainwindow->getPtr(ERecordTypes::LOGINS, profileNumber).data();
        QFutureWatcher<void>* watcher = _mainwindow->getWatcher(profileNumber, ERecordTypes::LOGINS);
        watcher->waitForFinished();
        QFuture<void> fut = QtConcurrent::run(s_exportRecords, this, ptr, path, md5file);
        watcher->setFuture(fut);
        _watchers.push_back(watcher);
    }

    if (ui->cacheCheckBox->isChecked() || ui->cacheFilesCheckBox->isChecked())
    {
        // Export cache
        auto ptr = _mainwindow->getPtr(ERecordTypes::CACHEFILES, profileNumber).data();
        QFutureWatcher<void>* watcher = _mainwindow->getWatcher(profileNumber, ERecordTypes::CACHEFILES);
        watcher->waitForFinished();
        QFuture<void> fut = QtConcurrent::run(s_exportCache, this, ptr, path, profileNumber, md5file);
        watcher->setFuture(fut);
        _watchers.push_back(watcher);
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
    this->exportData();
}

void Export::on_browseButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(0, ("Select Output Folder"), ui->pathInputText->text());
    if (path != "")
        ui->pathInputText->setText(path);
}


void Export::closeEvent(QCloseEvent *event)
{
    bool not_finished = false;

    for (auto& task : _watchers) {
        if (task->isRunning()) {
            not_finished = true;
            break;
        }
    }

    if (not_finished) {
        QMessageBox::StandardButton resBtn = QMessageBox::question(this, tr("Stop export"),
                                                                   tr("Stop export of data ?\n"),
                                                                   QMessageBox::Cancel | QMessageBox::Yes,
                                                                   QMessageBox::Yes);

        if (resBtn != QMessageBox::Yes) {
            event->ignore();
        }
        else {
            event->accept();

            //stop parsing
            dll_stopParsing(true);

            //wait for all tasks to finish
            for (auto& task : _watchers) {
                task->waitForFinished();
            }
        }
    }
    else {
        event->accept();
    }

    _mainwindow->updateTotalRecords();
}
