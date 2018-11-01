#ifndef GEODATARIVERSURVEYCROSSSECTIONEDITFROMPOINTDIALOG_H
#define GEODATARIVERSURVEYCROSSSECTIONEDITFROMPOINTDIALOG_H

#include "geodatarivercrosssection.h"

#include <QDialog>

namespace Ui {
class GeoDataRiverSurveyCrossSectionEditFromPointDialog;
}

class GeoDataRiverPathPoint;
class GeoDataRiverSurvey;
class GeoDataRiverSurveyCrosssectionWindow;

class QAbstractButton;

class GeoDataRiverSurveyCrossSectionEditFromPointDialog : public QDialog
{
	Q_OBJECT

public:
	GeoDataRiverSurveyCrossSectionEditFromPointDialog(int startIndex, GeoDataRiverPathPoint* point, GeoDataRiverSurvey* rs, GeoDataRiverSurveyCrosssectionWindow* w, QWidget *parent = nullptr);
	~GeoDataRiverSurveyCrossSectionEditFromPointDialog();

public slots:
	void accept() override;
	void reject() override;

private slots:
	void continueEdit();
	void handleButtonClick(QAbstractButton* button);

private:
	enum CrossCondition {
		Hit,
		Upper,
		Lower
	};

	void apply();

	void update(int* newIndex);
	void reset();

	GeoDataRiverCrosssection::Altitude buildAltitudeForDistance() const;
	GeoDataRiverCrosssection::Altitude buildAltitudeForCrossPoint(const GeoDataRiverCrosssection::Altitude& a1, const GeoDataRiverCrosssection::Altitude& a2) const;
	CrossCondition conditionForLine(const GeoDataRiverCrosssection::Altitude& alt);
	double heightForLine(double pos) const;
	double slope() const;
	void showWarningDialogForNotCrossing();

	int m_startIndex;
	GeoDataRiverPathPoint* m_point;
	GeoDataRiverSurvey* m_rs;
	GeoDataRiverSurveyCrosssectionWindow* m_window;
	bool m_applied;

	Ui::GeoDataRiverSurveyCrossSectionEditFromPointDialog *ui;
};

#endif // GEODATARIVERSURVEYCROSSSECTIONEDITFROMPOINTDIALOG_H
