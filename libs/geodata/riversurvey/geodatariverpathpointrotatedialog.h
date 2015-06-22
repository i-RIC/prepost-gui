#ifndef GEODATARIVERPATHPOINTROTATEDIALOG_H
#define GEODATARIVERPATHPOINTROTATEDIALOG_H

#include <QDialog>

namespace Ui
{
	class GeoDataRiverPathPointRotateDialog;
}

class GeoDataRiverSurvey;
class QAbstractButton;

class GeoDataRiverPathPointRotateDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataRiverPathPointRotateDialog(GeoDataRiverSurvey* rs, QWidget* parent = nullptr);
	~GeoDataRiverPathPointRotateDialog();
	void setCurrentRelativeAngle(double current);

public slots:
	void accept() override;
	void reject() override;

private slots:
	void relativeChange();
	void incrementChange();
	void handleButtonClick(QAbstractButton* button);

private:
	void doReset();
	void apply();

	Ui::GeoDataRiverPathPointRotateDialog* ui;
	double m_currentRelativeAngle;
	GeoDataRiverSurvey* m_rs;
	bool m_applyed;
};

#endif // GEODATARIVERPATHPOINTROTATEDIALOG_H
