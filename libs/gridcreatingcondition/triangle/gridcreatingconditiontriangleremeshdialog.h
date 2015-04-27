#ifndef GRIDCREATINGCONDITIONTRIANGLEREMESHDIALOG_H
#define GRIDCREATINGCONDITIONTRIANGLEREMESHDIALOG_H

#include <QDialog>

namespace Ui {
	class GridCreatingConditionTriangleRemeshDialog;
}

class GridCreatingConditionTriangleRemeshDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GridCreatingConditionTriangleRemeshDialog(QWidget *parent = nullptr);
	~GridCreatingConditionTriangleRemeshDialog();
	void setArea(double area);
	double area();
	void setRequired();
	void accept();
	void reject();
private:
	Ui::GridCreatingConditionTriangleRemeshDialog *ui;
	bool m_required;
};

#endif // GRIDCREATINGCONDITIONTRIANGLEREMESHDIALOG_H
