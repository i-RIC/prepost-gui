#ifndef GRIDCREATINGCONDITIONPOISSONCTRLPOINTSDIALOG_H
#define GRIDCREATINGCONDITIONPOISSONCTRLPOINTSDIALOG_H

#include <QDialog>

namespace Ui {
class GridCreatingConditionPoissonCtrlPointsDialog;
}

class GridCreatingConditionPoissonCtrlPointsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GridCreatingConditionPoissonCtrlPointsDialog(QWidget *parent = 0);
	~GridCreatingConditionPoissonCtrlPointsDialog();

	void setMaximum(int max);
	int value() const;

private:
	Ui::GridCreatingConditionPoissonCtrlPointsDialog *ui;
};

#endif // GRIDCREATINGCONDITIONPOISSONCTRLPOINTSDIALOG_H
