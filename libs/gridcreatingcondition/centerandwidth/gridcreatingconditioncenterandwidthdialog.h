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

	int iMax() const;
	int jMax() const;
	double width() const;
	double length() const;

	void setIMax(int i);
	void setJMax(int j);
	void setWidth(double w);
	void setLength(double l);

public slots:
	void setStreamWiseLabel();
	void setCrossStreamLabel();

private slots:
	void setStreamWiseNumber();
	void setCrossStreamNumber();
	void handleButtonClick(QAbstractButton* button);

signals:
	void applied(QDialog* d);

private:
	void apply();

	double m_length;
	Ui::GridCreatingConditionCenterAndWidthDialog* ui;
};

#endif // GRIDCREATINGCONDITIONCENTERANDWIDTHDIALOG_H
