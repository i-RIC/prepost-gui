#ifndef RAWDATAPOLYGONCOORDINATESEDITDIALOG_H
#define RAWDATAPOLYGONCOORDINATESEDITDIALOG_H

#include <QDialog>
#include <QVector>
#include <QVector2D>

namespace Ui
{
	class RawDataPolygonCoordinatesEditDialog;
}

class RawDataPolygon;
class QAbstractButton;
class QStandardItemModel;

class RawDataPolygonCoordinatesEditDialog : public QDialog
{
	Q_OBJECT

private:
	static const int defaultRowHeight = 20;

public:
	explicit RawDataPolygonCoordinatesEditDialog(RawDataPolygon* polygon, QWidget* parent = nullptr);
	~RawDataPolygonCoordinatesEditDialog();

public slots:
	void accept() override;
	void reject() override;

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	QVector<QVector2D> getCoords();
	void setupData();
	void apply();

	Ui::RawDataPolygonCoordinatesEditDialog* ui;
	RawDataPolygon* m_polygon;
	QStandardItemModel* m_model;
	bool m_applyed;
};

#endif // RAWDATAPOLYGONCOORDINATESEDITDIALOG_H
