#ifndef GRIDCREATINGCONDITIONLAPLACEDEPLOYSETTINGDIALOG_H
#define GRIDCREATINGCONDITIONLAPLACEDEPLOYSETTINGDIALOG_H

#include "gridcreatingconditionlaplace.h"

#include <QDialog>

namespace Ui {
class GridCreatingConditionLaplaceDeploySettingDialog;
}

class GridCreatingConditionLaplaceDeploySettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GridCreatingConditionLaplaceDeploySettingDialog(QWidget *parent = nullptr);
	~GridCreatingConditionLaplaceDeploySettingDialog();

	GridCreatingConditionLaplace::DivisionMode divisionMode();
	void setDivisionMode(GridCreatingConditionLaplace::DivisionMode mode);

	double commonRatio();
	void setCommonRatio(double ratio);

private:
	Ui::GridCreatingConditionLaplaceDeploySettingDialog *ui;
};

#endif // GRIDCREATINGCONDITIONLAPLACEDEPLOYSETTINGDIALOG_H
