#ifndef GEODATAPOLYGONCOORDINATESEDITDIALOG_H
#define GEODATAPOLYGONCOORDINATESEDITDIALOG_H

#include <QDialog>
#include <QVector>
#include <QVector2D>

namespace Ui
{
	class GeoDataPolygonCoordinatesEditDialog;
}

class GeoDataPolygon;
class QAbstractButton;
class QStandardItemModel;

class GeoDataPolygonCoordinatesEditDialog : public QDialog
{
	Q_OBJECT

private:
	static const int defaultRowHeight = 20;

public:
	explicit GeoDataPolygonCoordinatesEditDialog(GeoDataPolygon* polygon, QWidget* parent = nullptr);
	~GeoDataPolygonCoordinatesEditDialog();

public slots:
	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	QVector<QVector2D> getCoords();
	void setupData();
	void apply();

	Ui::GeoDataPolygonCoordinatesEditDialog* ui;
	GeoDataPolygon* m_polygon;
	QStandardItemModel* m_model;
	bool m_applyed;
};

#endif // GEODATAPOLYGONCOORDINATESEDITDIALOG_H
