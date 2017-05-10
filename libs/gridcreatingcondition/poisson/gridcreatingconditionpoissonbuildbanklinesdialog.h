#ifndef GRIDCREATINGCONDITIONPOISSONBUILDBANKLINESDIALOG_H
#define GRIDCREATINGCONDITIONPOISSONBUILDBANKLINESDIALOG_H

#include <QDialog>

namespace Ui {
class GridCreatingConditionPoissonBuildBankLinesDialog;
}

class GridCreatingConditionPoissonBuildBankLinesDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GridCreatingConditionPoissonBuildBankLinesDialog(QWidget *parent = 0);
	~GridCreatingConditionPoissonBuildBankLinesDialog();

	double leftBankDistance() const;
	void setLeftBankDistance(double dist);

	double rightBankDistance() const;
	void setRightBankDistance(double dist);

private:
	Ui::GridCreatingConditionPoissonBuildBankLinesDialog *ui;
};

#endif // GRIDCREATINGCONDITIONPOISSONBUILDBANKLINESDIALOG_H
