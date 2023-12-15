#ifndef GRIDCREATINGCONDITIONCENTERANDWIDTHDIALOG_H
#define GRIDCREATINGCONDITIONCENTERANDWIDTHDIALOG_H


#include <QDialog>

namespace Ui
{
	class GridCreatingConditionCenterAndWidthDialog;
}

class QAbstractButton;

class GridCreatingConditionCenterAndWidthDialog : public QDialog
{
	Q_OBJECT

public:
	GridCreatingConditionCenterAndWidthDialog(QWidget* parent = nullptr);
	~GridCreatingConditionCenterAndWidthDialog();

	void setReadOnly(bool readOnly);
	int iMax() const;
	int jMax() const;
	double width() const;

	void setIMax(int i);
	void setJMax(int j);
	void setWidth(double w);
	void setLength(double l);

private slots:
	void updateDI();
	void updateDJ();
	void handleNJChange();
	void handleButtonClick(QAbstractButton* button);

signals:
	void applied(QDialog* d);

private:
	void apply();

	double m_length;
	Ui::GridCreatingConditionCenterAndWidthDialog* ui;
};

#endif // GRIDCREATINGCONDITIONCENTERANDWIDTHDIALOG_H
