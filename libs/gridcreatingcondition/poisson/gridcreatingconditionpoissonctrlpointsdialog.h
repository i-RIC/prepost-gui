#ifndef GRIDCREATINGCONDITIONPOISSONCTRLPOINTSDIALOG_H
#define GRIDCREATINGCONDITIONPOISSONCTRLPOINTSDIALOG_H

#include <QDialog>

namespace Ui {
class GridCreatingConditionPoissonCtrlPointsDialog;
}

class QLocale;

class GridCreatingConditionPoissonCtrlPointsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GridCreatingConditionPoissonCtrlPointsDialog(QWidget *parent = 0);
	~GridCreatingConditionPoissonCtrlPointsDialog();

	void setLocale(const QLocale& locale);
	void setMaximum(int max);
	int value() const;

private:
	Ui::GridCreatingConditionPoissonCtrlPointsDialog *ui;
};

#endif // GRIDCREATINGCONDITIONPOISSONCTRLPOINTSDIALOG_H
