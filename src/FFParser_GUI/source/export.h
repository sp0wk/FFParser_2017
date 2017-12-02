#ifndef EXPORT_H
#define EXPORT_H

#include <QDialog>
#include <vector>
#include <string>
#include <QDebug>

namespace Ui {
class Export;
}

class Export : public QDialog
{
    Q_OBJECT

public:
    explicit Export(QWidget *parent = 0);
    ~Export();

    void setProfile(const char *);
    void setProfileCombobox();

private:
    Ui::Export *ui;
    std::vector<const char *> profiles;

};

#endif // EXPORT_H
