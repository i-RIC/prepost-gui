#ifndef GRIDCREATINGCONDITIONLAPLACEDIVISIONSETTINGDIALOG_H
#define GRIDCREATINGCONDITIONLAPLACEDIVISIONSETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class GridCreatingConditionLaplaceDivisionSettingDialog;
}

class GridCreatingConditionLaplaceDivisionSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GridCreatingConditionLaplaceDivisionSettingDialog(QWidget *parent = nullptr);
	~GridCreatingConditionLaplaceDivisionSettingDialog();

	int divisionNumber() const;
	void setDivisionNumber(int num);

private:
	Ui::GridCreatingConditionLaplaceDivisionSettingDialog *ui;
};

#endif // GRIDCREATINGCONDITIONLAPLACEDIVISIONSETTINGDIALOG_H
