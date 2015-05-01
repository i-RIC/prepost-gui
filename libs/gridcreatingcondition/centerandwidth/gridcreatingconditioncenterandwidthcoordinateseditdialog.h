#ifndef GRICCREATINGCONDITIONCENTERANDWIDTHCOORDINATESEDITDIALOG_H
#define GRICCREATINGCONDITIONCENTERANDWIDTHCOORDINATESEDITDIALOG_H

#include <QDialog>
#include <QVector>
#include <QVector2D>

namespace Ui
{
	class GridCreatingConditionCenterAndWidthCoordinatesEditDialog;
}

class GridCreatingConditionCenterAndWidth;
class QAbstractButton;
class QStandardItemModel;

class GridCreatingConditionCenterAndWidthCoordinatesEditDialog : public QDialog
{
	Q_OBJECT
private:
	static const int defaultRowHeight = 20;
public:
	explicit GridCreatingConditionCenterAndWidthCoordinatesEditDialog(GridCreatingConditionCenterAndWidth* cond, QWidget* parent = nullptr);
	~GridCreatingConditionCenterAndWidthCoordinatesEditDialog();
public slots:
	void accept();
	void reject();
private slots:
	void handleButtonClick(QAbstractButton* button);
private:
	QVector<QVector2D> getCoords();
	void setupData();
	void apply();

	Ui::GridCreatingConditionCenterAndWidthCoordinatesEditDialog* ui;
	GridCreatingConditionCenterAndWidth* m_condition;
	QStandardItemModel* m_model;
	bool m_applyed;
};

#endif // GRICCREATINGCONDITIONCENTERANDWIDTHCOORDINATESEDITDIALOG_H
