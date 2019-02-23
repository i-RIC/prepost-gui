#ifndef GRIDCREATINGCONDITIONLAPLACECTRLPOINTSDIALOG_H
#define GRIDCREATINGCONDITIONLAPLACECTRLPOINTSDIALOG_H

#include <QDialog>

namespace Ui {
class GridCreatingConditionLaplaceCtrlPointsDialog;
}

class QLocale;

class GridCreatingConditionLaplaceCtrlPointsDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GridCreatingConditionLaplaceCtrlPointsDialog(QWidget *parent = 0);
	~GridCreatingConditionLaplaceCtrlPointsDialog();

	void setLocale(const QLocale& locale);
	void setMaximum(int max);
	int value() const;

public slots:
	void reject() override;

private:
	Ui::GridCreatingConditionLaplaceCtrlPointsDialog *ui;
};

#endif // GRIDCREATINGCONDITIONLAPLACECTRLPOINTSDIALOG_H
