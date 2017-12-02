#include "export.h"
#include "ui_export.h"

Export::Export(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Export)
{
    ui->setupUi(this);
}

Export::~Export()
{
    delete ui;
}

void Export::setProfile(const char *nameProfile)
{
    profiles.push_back(nameProfile);
}

void Export::setProfileCombobox()
{
    for (size_t i = 0; i < profiles.size(); ++i)
        ui->chooseProfile->addItem(profiles[i], i);
}
