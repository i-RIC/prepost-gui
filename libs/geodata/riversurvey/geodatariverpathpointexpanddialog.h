#ifndef GEODATARIVERPATHPOINTEXPANDDIALOG_H
#define GEODATARIVERPATHPOINTEXPANDDIALOG_H

#include <QDialog>
#include <QList>

namespace Ui
{
	class GeoDataRiverPathPointExpandDialog;
}

class GeoDataRiverPathPoint;
class GeoDataRiverSurvey;
class QAbstractButton;

class GeoDataRiverPathPointExpandDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataRiverPathPointExpandDialog(GeoDataRiverSurvey* rs, QWidget* parent = 0);
	~GeoDataRiverPathPointExpandDialog();

public slots:
	void accept() override;
	void reject() override;

private slots:
	void distanceChanged();
	void incrementChanged();
	void ratioChanged();
	void handleButtonClick(QAbstractButton* button);

private:
	void apply();
	void doReset();
	void initializeWidgets();
	void updateButtonStatus(bool enable);
	double getRatio(GeoDataRiverPathPoint* p);
	void getRatios(QList<double>& ratios);
	Ui::GeoDataRiverPathPointExpandDialog* ui;
	QList<GeoDataRiverPathPoint*> m_points;
	GeoDataRiverSurvey* m_rs;
	double m_originalDistance;
	double m_minDistance;
	bool m_applyed;
};

#endif // GEODATARIVERPATHPOINTEXPANDDIALOG_H
