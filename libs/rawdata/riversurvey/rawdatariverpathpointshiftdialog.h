#ifndef RAWDATARIVERPATHPOINTSHIFTDIALOG_H
#define RAWDATARIVERPATHPOINTSHIFTDIALOG_H

#include <QDialog>

namespace Ui
{
	class RawDataRiverPathPointShiftDialog;
}

class QAbstractButton;
class RawDataRiverSurvey;

class RawDataRiverPathPointShiftDialog : public QDialog
{
	Q_OBJECT

public:
	explicit RawDataRiverPathPointShiftDialog(RawDataRiverSurvey* rs, QWidget* parent = nullptr);
	~RawDataRiverPathPointShiftDialog();
public slots:
	void accept() override;
	void reject() override;
private slots:
	void handleButtonClick(QAbstractButton* button);
	void checkShiftValue(double shiftVal);
private:
	void doReset();
	void apply();
	void customGraphicsUpdate();
private:

	Ui::RawDataRiverPathPointShiftDialog* ui;
	RawDataRiverSurvey* m_rs;
	double m_leftMax;
	double m_rightMax;
	bool m_applyed;
};

#endif // RAWDATARIVERPATHPOINTSHIFTDIALOG_H
