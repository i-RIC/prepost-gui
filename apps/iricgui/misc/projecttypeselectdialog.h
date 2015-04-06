#ifndef PROJECTTYPESELECTDIALOG_H
#define PROJECTTYPESELECTDIALOG_H

#include <QDialog>

namespace Ui {
    class ProjectTypeSelectDialog;
}

class ProjectTypeSelectDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProjectTypeSelectDialog(QWidget *parent = 0);
    ~ProjectTypeSelectDialog();
	bool folderProject();
private:
    Ui::ProjectTypeSelectDialog *ui;
};

#endif // PROJECTTYPESELECTDIALOG_H
