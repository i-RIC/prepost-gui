#ifndef GRIDCREATINGCONDITIONLAPLACEWHOLEREGIONDIVISIONSETTINGDIALOG_H
#define GRIDCREATINGCONDITIONLAPLACEWHOLEREGIONDIVISIONSETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class GridCreatingConditionLaplaceWholeRegionDivisionSettingDialog;
}

class GridCreatingConditionLaplaceWholeRegionDivisionSettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GridCreatingConditionLaplaceWholeRegionDivisionSettingDialog(QWidget *parent = 0);
	~GridCreatingConditionLaplaceWholeRegionDivisionSettingDialog();

	int iDiv() const;
	void setIDiv(int div);
	void setIDivMin(int div);

	int jDiv() const;
	void setJDiv(int div);
	void setJDivMin(int div);

	void setILength(double len);
	void setJLength(double len);

	void setGridCreateMode();

private slots:
	void updateDI();
	void updateDJ();

private:
	Ui::GridCreatingConditionLaplaceWholeRegionDivisionSettingDialog *ui;

	double m_iLength;
	double m_jLength;
};

#endif // GRIDCREATINGCONDITIONLAPLACEWHOLEREGIONDIVISIONSETTINGDIALOG_H
