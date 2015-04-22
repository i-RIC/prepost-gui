#ifndef GRIDCREATINGCONDITIONTRIANGLESETTINGDIALOG_H
#define GRIDCREATINGCONDITIONTRIANGLESETTINGDIALOG_H

#include <QDialog>

namespace Ui {
	class GridCreatingConditionTriangleSettingDialog;
}

class GridCreatingConditionTriangleSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GridCreatingConditionTriangleSettingDialog(QWidget *parent = nullptr);
	~GridCreatingConditionTriangleSettingDialog();
	bool angleConstraint();
	void setAngleConstraint(bool c);
	double angle();
	void setAngle(double angle);
	bool areaConstraint();
	void setAreaConstraint(bool c);
	double area();
	void setArea(double a);
private:
	Ui::GridCreatingConditionTriangleSettingDialog *ui;
};

#endif // GRIDCREATINGCONDITIONTRIANGLESETTINGDIALOG_H
