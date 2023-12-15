#ifndef GRIDCREATINGCONDITIONRECTANGULARREGIONSETTINGDIALOG_H
#define GRIDCREATINGCONDITIONRECTANGULARREGIONSETTINGDIALOG_H

#include <QDialog>

class QAbstractButton;

namespace Ui
{
	class GridCreatingConditionRectangularRegionSettingDialog;
}

class GridCreatingConditionRectangularRegion;

class GridCreatingConditionRectangularRegionSettingDialog : public QDialog
{
	Q_OBJECT

public:
	GridCreatingConditionRectangularRegionSettingDialog(GridCreatingConditionRectangularRegion* cond, QWidget* parent = nullptr);
	~GridCreatingConditionRectangularRegionSettingDialog();

	void setReadOnly(bool readOnly);
	void setXMin(double xmin);
	void setXMax(double xmax);
	void setYMin(double ymin);
	void setYMax(double ymax);
	void setStepSize(double size);

public slots:
	void accept() override;

private slots:
	void updateResultDisplays();

	void checkXMin();
	void checkXMax();
	void checkYMin();
	void checkYMax();

	void handleButtonClick(QAbstractButton* button);

protected:
	void showEvent(QShowEvent*) override;

private:
	Ui::GridCreatingConditionRectangularRegionSettingDialog* ui;
	GridCreatingConditionRectangularRegion* m_condition;
	void apply();
};

#endif // GRIDCREATINGCONDITIONRECTANGULARREGIONSETTINGDIALOG_H
