#ifndef GEODATARIVERCROSSSECTIONALTITUDEMOVEDIALOG_H
#define GEODATARIVERCROSSSECTIONALTITUDEMOVEDIALOG_H

#include <QDialog>

class QAbstractButton;
class GeoDataRiverPathPoint;
class GeoDataRiverSurvey;
class GeoDataRiverSurveyCrosssectionWindow;

namespace Ui
{
	class GeoDataRiverCrosssectionAltitudeMoveDialog;
}

class GeoDataRiverCrosssectionAltitudeMoveDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataRiverCrosssectionAltitudeMoveDialog(GeoDataRiverPathPoint* point, int from, int to, GeoDataRiverSurvey* rs, GeoDataRiverSurveyCrosssectionWindow* w, QWidget* parent = nullptr);
	~GeoDataRiverCrosssectionAltitudeMoveDialog();

public slots:
	void accept() override;
	void reject() override;

private slots:
	void hOffsetChange();
	void handleButtonClick(QAbstractButton* button);

private:
	void setLimits();
	void doReset();
	void apply();
	void updateCrosssection();

	Ui::GeoDataRiverCrosssectionAltitudeMoveDialog* ui;
	bool m_leftLimitSet;
	bool m_rightLimitSet;
	double m_leftLimit;
	double m_rightLimit;

	int m_from;
	int m_to;

	GeoDataRiverSurveyCrosssectionWindow* m_window;
	GeoDataRiverPathPoint* m_point;
	GeoDataRiverSurvey* m_rs;
	bool m_applyed;
};

#endif // GEODATARIVERCROSSSECTIONALTITUDEMOVEDIALOG_H
