#include "exportcachefiledialog.h"
#include "ui_exportcachefiledialog.h"
#include <QFileDialog>
#include <QString>


ExportCacheFileDialog::ExportCacheFileDialog(MainWindow *parent) :
    QDialog(parent),
    ui(new Ui::ExportCacheFileDialog),
    _mainwindow(parent)
{
    ui->setupUi(this);
    ui->pathInputText->setText(QCoreApplication::applicationFilePath());
    this->setWindowTitle(tr("Cache file export"));
    this->setModal(true);
}

ExportCacheFileDialog::~ExportCacheFileDialog()
{
    delete ui;
}

void ExportCacheFileDialog::on_browseButton_clicked()
{
    QString path = QFileDialog::getExistingDirectory(0, (tr("Select Output Folder")), ui->pathInputText->text());
    if (path != "")
        ui->pathInputText->setText(path);
}


void ExportCacheFileDialog::exportCacheFile()
{
    QString path = ui->pathInputText->text();
    bool md5file = ui->calculateMD5CheckBox->isChecked();

    _mainwindow->exportSelectedFile(path.toStdString().c_str(), md5file);

    this->hide();
}


void ExportCacheFileDialog::on_exportButton_clicked()
{
    exportCacheFile();
}
