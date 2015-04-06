#ifndef RAWDATARIVERPATHPOINTROTATEDIALOG_H
#define RAWDATARIVERPATHPOINTROTATEDIALOG_H

#include <QDialog>

namespace Ui {
	class RawDataRiverPathPointRotateDialog;
}

class RawDataRiverSurvey;
class QAbstractButton;

class RawDataRiverPathPointRotateDialog : public QDialog
{
	Q_OBJECT

public:
	explicit RawDataRiverPathPointRotateDialog(RawDataRiverSurvey* rs, QWidget *parent = 0);
	~RawDataRiverPathPointRotateDialog();
	void setCurrentRelativeAngle(double current);
public slots:
	void accept();
	void reject();
private slots:
	void relativeChange();
	void incrementChange();
	void handleButtonClick(QAbstractButton* button);
private:
	void doReset();
	void apply();

	Ui::RawDataRiverPathPointRotateDialog *ui;
	double m_currentRelativeAngle;
	RawDataRiverSurvey* m_rs;
	bool m_applyed;
};

#endif // RAWDATARIVERPATHPOINTROTATEDIALOG_H
