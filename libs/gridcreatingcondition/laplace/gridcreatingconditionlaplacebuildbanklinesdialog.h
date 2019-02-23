#ifndef GRIDCREATINGCONDITIONLAPLACEBUILDBANKLINESDIALOG_H
#define GRIDCREATINGCONDITIONLAPLACEBUILDBANKLINESDIALOG_H

#include <QDialog>

namespace Ui {
class GridCreatingConditionLaplaceBuildBankLinesDialog;
}

class GridCreatingConditionLaplaceBuildBankLinesDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GridCreatingConditionLaplaceBuildBankLinesDialog(QWidget *parent = 0);
	~GridCreatingConditionLaplaceBuildBankLinesDialog();

	double leftBankDistance() const;
	void setLeftBankDistance(double dist);

	double rightBankDistance() const;
	void setRightBankDistance(double dist);

private:
	Ui::GridCreatingConditionLaplaceBuildBankLinesDialog *ui;
};

#endif // GRIDCREATINGCONDITIONLAPLACEBUILDBANKLINESDIALOG_H
