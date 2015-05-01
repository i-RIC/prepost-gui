#ifndef GRIDCREATINGCONDITIONCOMPOUNDCHANNELSETTINGDIALOG_H
#define GRIDCREATINGCONDITIONCOMPOUNDCHANNELSETTINGDIALOG_H

#include <QDialog>
#include <QMap>

class Grid;

namespace Ui
{
	class GridCreatingConditionCompoundChannelSettingDialog;
}

class GridCreatingConditionCompoundChannelSettingDialog : public QDialog
{
	Q_OBJECT
public:
	GridCreatingConditionCompoundChannelSettingDialog(QWidget* parent = 0);
	~GridCreatingConditionCompoundChannelSettingDialog();

	void setStreamWiseDivision(int div);
	void setLeftDivision(int div);
	void setRightDivision(int div);
	void setCenterDivision(int div);

	void setRelaxation(double r);
	void setIterations(int i);

	int streamWiseDivision();
	int leftDivision();
	int rightDivision();
	int centerDivision();

	double relaxation();
	int iterations();
public slots:
	void toggleDetailCondition();
private:
	Ui::GridCreatingConditionCompoundChannelSettingDialog* ui;
};

#endif // GRIDCREATINGCONDITIONCOMPOUNDCHANNELSETTINGDIALOG_H
