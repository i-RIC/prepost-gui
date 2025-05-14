#ifndef GEODATARIVERSURVEYCROSSSECTIONWINDOW_POINTADDDIALOG_H
#define GEODATARIVERSURVEYCROSSSECTIONWINDOW_POINTADDDIALOG_H

#include "../geodatariverpathpointodndata.h"
#include "../geodatariversurveycrosssectionwindow.h"

#include <QDialog>

namespace Ui {
class GeoDataRiverSurveyCrosssectionWindow_PointAddDialog;
}

class GeoDataRiverSurveyCrosssectionWindow::PointAddDialog : public QDialog
{
	Q_OBJECT

public:
	explicit PointAddDialog(GeoDataRiverPathPoint* p, GeoDataRiverSurvey* rs, GeoDataRiverSurveyCrosssectionWindow *parent);
	~PointAddDialog();

public slots:
	void accept() override;
	void reject() override;

	void setPoint(const QPointF& position);

private slots:
	void handleButtonClick(QAbstractButton* button);

private:
	void apply();

	GeoDataRiverCrosssection::AltitudeList createNewList(int* insertPosition) const;

	GeoDataRiverPathPoint* m_point;
	GeoDataRiverSurvey* m_rs;
	bool m_applyed;
	GeoDataRiverCrosssection::AltitudeList m_alist;
	GeoDataRiverPathPointOdnData m_odn;

	Ui::GeoDataRiverSurveyCrosssectionWindow_PointAddDialog *ui;
};

#endif // GEODATARIVERSURVEYCROSSSECTIONWINDOW_POINTADDDIALOG_H
