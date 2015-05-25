#ifndef GRIDCREATINGCONDITIONTRIANGLEPOLYLINECOORDINATESEDITDIALOG_H
#define GRIDCREATINGCONDITIONTRIANGLEPOLYLINECOORDINATESEDITDIALOG_H

#include <QDialog>
#include <QVector>
#include <QVector2D>

namespace Ui
{
	class GridCreatingConditionTrianglePolyLineCoordinatesEditDialog;
}

class GridCreatingConditionTriangle;
class QAbstractButton;
class QStandardItemModel;

class GridCreatingConditionTrianglePolyLineCoordinatesEditDialog : public QDialog
{
	Q_OBJECT
private:
	static const int defaultRowHeight = 20;
public:
	explicit GridCreatingConditionTrianglePolyLineCoordinatesEditDialog(GridCreatingConditionTriangle* polygon, QWidget* parent = nullptr);
	~GridCreatingConditionTrianglePolyLineCoordinatesEditDialog();
public slots:
	void accept() override;
	void reject() override;
private slots:
	void handleButtonClick(QAbstractButton* button);
private:
	QVector<QVector2D> getCoords();
	void setupData();
	void apply();

	Ui::GridCreatingConditionTrianglePolyLineCoordinatesEditDialog* ui;
	GridCreatingConditionTriangle* m_polygon;
	QStandardItemModel* m_model;
	bool m_applyed;
};

#endif // GRIDCREATINGCONDITIONTRIANGLEPOLYLINECOORDINATESEDITDIALOG_H
