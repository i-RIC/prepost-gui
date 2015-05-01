#ifndef PROJECTPROPERTYDIALOG_H
#define PROJECTPROPERTYDIALOG_H

#include <QDialog>

namespace Ui
{
	class ProjectPropertyDialog;
}

class ProjectData;

class ProjectPropertyDialog : public QDialog
{
	Q_OBJECT

public:
	explicit ProjectPropertyDialog(QWidget* parent = nullptr);
	~ProjectPropertyDialog();
	void setProjectData(ProjectData* data);
private:
	Ui::ProjectPropertyDialog* ui;
};

#endif // PROJECTPROPERTYDIALOG_H
