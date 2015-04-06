#ifndef RAWDATARIVERCROSSSECTIONALTITUDEMOVEDIALOG_H
#define RAWDATARIVERCROSSSECTIONALTITUDEMOVEDIALOG_H

#include <QDialog>

class QAbstractButton;
class RawDataRiverPathPoint;
class RawDataRiverSurvey;
class RawDataRiverSurveyCrosssectionWindow;

namespace Ui {
	class RawDataRiverCrosssectionAltitudeMoveDialog;
}

class RawDataRiverCrosssectionAltitudeMoveDialog : public QDialog
{
	Q_OBJECT
public:
	explicit RawDataRiverCrosssectionAltitudeMoveDialog(RawDataRiverPathPoint* point, int from, int to, RawDataRiverSurvey* rs, RawDataRiverSurveyCrosssectionWindow* w, QWidget *parent = 0);
	~RawDataRiverCrosssectionAltitudeMoveDialog();
public slots:
	void accept();
	void reject();
private slots:
	void hOffsetChange();
	void handleButtonClick(QAbstractButton* button);
private:
	void setLimits();
	void doReset();
	void apply();
	void updateCrosssection();

	Ui::RawDataRiverCrosssectionAltitudeMoveDialog *ui;
	bool m_leftLimitSet;
	bool m_rightLimitSet;
	double m_leftLimit;
	double m_rightLimit;

	int m_from;
	int m_to;

	RawDataRiverSurveyCrosssectionWindow* m_window;
	RawDataRiverPathPoint* m_point;
	RawDataRiverSurvey* m_rs;
	bool m_applyed;
};

#endif // RAWDATARIVERCROSSSECTIONALTITUDEMOVEDIALOG_H
