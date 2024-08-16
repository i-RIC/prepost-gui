#ifndef GRIDCREATINGCONDITIONLAPLACEINTERPOLATESETTINGDIALOG_H
#define GRIDCREATINGCONDITIONLAPLACEINTERPOLATESETTINGDIALOG_H

#include "gridcreatingconditionlaplace.h"

#include <QDialog>

namespace Ui {
class GridCreatingConditionLaplaceInterpolateSettingDialog;
}

class GridCreatingConditionLaplaceInterpolateSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GridCreatingConditionLaplaceInterpolateSettingDialog(QWidget *parent = nullptr);
	~GridCreatingConditionLaplaceInterpolateSettingDialog();

	GridCreatingConditionLaplace::InterpolationType interpolationType() const;
	void setInterpolationType(GridCreatingConditionLaplace::InterpolationType type);

	bool thisLineOnly() const;

private:
	Ui::GridCreatingConditionLaplaceInterpolateSettingDialog *ui;
};

#endif // GRIDCREATINGCONDITIONLAPLACEINTERPOLATESETTINGDIALOG_H
