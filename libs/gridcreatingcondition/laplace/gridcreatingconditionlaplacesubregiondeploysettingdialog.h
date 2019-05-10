#ifndef GRIDCREATINGCONDITIONLAPLACESUBREGIONDEPLOYSETTINGDIALOG_H
#define GRIDCREATINGCONDITIONLAPLACESUBREGIONDEPLOYSETTINGDIALOG_H

#include "gridcreatingconditionlaplace.h"

#include <QDialog>

namespace Ui {
class GridCreatingConditionLaplaceSubRegionDeploySettingDialog;
}

class GridCreatingConditionLaplaceSubRegionDeploySettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GridCreatingConditionLaplaceSubRegionDeploySettingDialog(GridCreatingConditionLaplace* cond, QWidget *parent = nullptr);
	~GridCreatingConditionLaplaceSubRegionDeploySettingDialog();

	void setParameter(GridCreatingConditionLaplace::DeployParameter param);
	GridCreatingConditionLaplace::DeployParameter parameter() const;

public slots:
	void accept() override;
	void reject() override;

private slots:
	void requestUpdate();

private:
	Ui::GridCreatingConditionLaplaceSubRegionDeploySettingDialog *ui;

	GridCreatingConditionLaplace::DeployParameter m_oldParameter;
	GridCreatingConditionLaplace* m_condition;
};

#endif // GRIDCREATINGCONDITIONLAPLACESUBREGIONDEPLOYSETTINGDIALOG_H
