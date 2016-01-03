#ifndef GEODATARIVERSURVEYDISPLAYSETTINGDIALOG_H
#define GEODATARIVERSURVEYDISPLAYSETTINGDIALOG_H

#include <QDialog>

namespace Ui
{
	class GeoDataRiverSurveyDisplaySettingDialog;
}

class GeoDataRiverSurveyDisplaySetting;

class GeoDataRiverSurveyDisplaySettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataRiverSurveyDisplaySettingDialog(QWidget* parent = nullptr);
	~GeoDataRiverSurveyDisplaySettingDialog();

	GeoDataRiverSurveyDisplaySetting setting() const;
	void setSetting(const GeoDataRiverSurveyDisplaySetting& setting);

private:
	Ui::GeoDataRiverSurveyDisplaySettingDialog* ui;
};

#endif // GEODATARIVERSURVEYDISPLAYSETTINGDIALOG_H
