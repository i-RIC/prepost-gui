#ifndef RAWDATARIVERPATHPOINTEXPANDDIALOG_H
#define RAWDATARIVERPATHPOINTEXPANDDIALOG_H

#include <QDialog>
#include <QList>

namespace Ui
{
	class RawDataRiverPathPointExpandDialog;
}

class RawDataRiverPathPoint;
class RawDataRiverSurvey;
class QAbstractButton;

class RawDataRiverPathPointExpandDialog : public QDialog
{
	Q_OBJECT

public:
	explicit RawDataRiverPathPointExpandDialog(RawDataRiverSurvey* rs, QWidget* parent = 0);
	~RawDataRiverPathPointExpandDialog();
public slots:
	void accept();
	void reject();
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
	double getRatio(RawDataRiverPathPoint* p);
	void getRatios(QList<double>& ratios);
	Ui::RawDataRiverPathPointExpandDialog* ui;
	QList<RawDataRiverPathPoint*> m_points;
	RawDataRiverSurvey* m_rs;
	double m_originalDistance;
	double m_minDistance;
	bool m_applyed;
};

#endif // RAWDATARIVERPATHPOINTEXPANDDIALOG_H
