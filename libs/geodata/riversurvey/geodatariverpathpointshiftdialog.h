#ifndef GEODATARIVERPATHPOINTSHIFTDIALOG_H
#define GEODATARIVERPATHPOINTSHIFTDIALOG_H

#include <QDialog>

namespace Ui
{
	class GeoDataRiverPathPointShiftDialog;
}

class QAbstractButton;
class GeoDataRiverSurvey;

class GeoDataRiverPathPointShiftDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataRiverPathPointShiftDialog(GeoDataRiverSurvey* rs, QWidget* parent = nullptr);
	~GeoDataRiverPathPointShiftDialog();

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

	Ui::GeoDataRiverPathPointShiftDialog* ui;
	GeoDataRiverSurvey* m_rs;
	double m_leftMax;
	double m_rightMax;
	bool m_applyed;
};

#endif // GEODATARIVERPATHPOINTSHIFTDIALOG_H
