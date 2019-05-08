#ifndef GRIDCREATINGCONDITIONLAPLACEDIVISIONSETTINGDIALOG_H
#define GRIDCREATINGCONDITIONLAPLACEDIVISIONSETTINGDIALOG_H

#include "gridcreatingconditionlaplace.h"

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

	GridCreatingConditionLaplace::DivisionMode divisionMode() const;
	void setDivisionMode(GridCreatingConditionLaplace::DivisionMode mode);

	double commonRatio() const;
	void setCommonRatio(double ratio);

	bool thisLineOnly() const;

private:
	Ui::GridCreatingConditionLaplaceDivisionSettingDialog *ui;
};

#endif // GRIDCREATINGCONDITIONLAPLACEDIVISIONSETTINGDIALOG_H
