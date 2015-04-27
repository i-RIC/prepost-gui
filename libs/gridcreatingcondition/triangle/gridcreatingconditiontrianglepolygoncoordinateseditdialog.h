#ifndef GRIDCREATINGCONDITIONTRIANGLEPOLYGONCOORDINATESEDITDIALOG_H
#define GRIDCREATINGCONDITIONTRIANGLEPOLYGONCOORDINATESEDITDIALOG_H

#include <QDialog>
#include <QVector>
#include <QVector2D>

namespace Ui {
	class GridCreatingConditionTrianglePolygonCoordinatesEditDialog;
}

class GridCreatingConditionTriangle;
class QAbstractButton;
class QStandardItemModel;

class GridCreatingConditionTrianglePolygonCoordinatesEditDialog : public QDialog
{
	Q_OBJECT
private:
	static const int defaultRowHeight = 20;
public:
	explicit GridCreatingConditionTrianglePolygonCoordinatesEditDialog(GridCreatingConditionTriangle* polygon, QWidget *parent = nullptr);
	~GridCreatingConditionTrianglePolygonCoordinatesEditDialog();
public slots:
	void accept();
	void reject();
private slots:
	void handleButtonClick(QAbstractButton* button);
private:
	QVector<QVector2D> getCoords();
	void setupData();
	void apply();

	Ui::GridCreatingConditionTrianglePolygonCoordinatesEditDialog *ui;
	GridCreatingConditionTriangle* m_polygon;
	QStandardItemModel* m_model;
	bool m_applyed;
};

#endif // GRIDCREATINGCONDITIONTRIANGLEPOLYGONCOORDINATESEDITDIALOG_H
