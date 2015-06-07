#ifndef GRIDCREATINGCONDITIONRECTANGULARREGIONLONLATSETTINGDIALOG_H
#define GRIDCREATINGCONDITIONRECTANGULARREGIONLONLATSETTINGDIALOG_H

#include <QDialog>

class QAbstractButton;

namespace Ui
{
	class GridCreatingConditionRectangularRegionLonLatSettingDialog;
}

class GridCreatingConditionRectangularRegionLonLat;

class GridCreatingConditionRectangularRegionLonLatSettingDialog : public QDialog
{
	Q_OBJECT

public:
	GridCreatingConditionRectangularRegionLonLatSettingDialog(GridCreatingConditionRectangularRegionLonLat* cond, QWidget* parent = nullptr);
	~GridCreatingConditionRectangularRegionLonLatSettingDialog();

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
	Ui::GridCreatingConditionRectangularRegionLonLatSettingDialog* ui;
	GridCreatingConditionRectangularRegionLonLat* m_condition;
	void apply();
};

#endif // GRIDCREATINGCONDITIONRECTANGULARREGIONLONLATSETTINGDIALOG_H
