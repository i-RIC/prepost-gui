#ifndef INPUTCONDITIONCGNSFILESELECTDIALOG_H
#define INPUTCONDITIONCGNSFILESELECTDIALOG_H

#include <QDialog>

namespace Ui {
class InputConditionCgnsFileSelectDialog;
}

class InputConditionCgnsFile;
class InputConditionWidgetSet;

class InputConditionCgnsFileSelectDialog : public QDialog
{
	Q_OBJECT

public:
	InputConditionCgnsFileSelectDialog(QString* cgnsFileForGrid, InputConditionWidgetSet* wset, QWidget *parent);
	~InputConditionCgnsFileSelectDialog();

	InputConditionCgnsFile* firstCgnsFile() const;

public slots:
	void accept() override;

private slots:
	void checkStatus();

private:
	std::vector<InputConditionCgnsFile*> m_cgnsFiles;

	QString* m_cgnsFileForGrid;
	InputConditionWidgetSet* m_widgetSet;
	Ui::InputConditionCgnsFileSelectDialog *ui;
};

#endif // INPUTCONDITIONCGNSFILESELECTDIALOG_H
