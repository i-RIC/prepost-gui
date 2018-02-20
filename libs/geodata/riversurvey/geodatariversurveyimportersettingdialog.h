#ifndef GEODATARIVERSURVEYIMPORTERSETTINGDIALOG_H
#define GEODATARIVERSURVEYIMPORTERSETTINGDIALOG_H

#include <QDialog>

namespace Ui
{
	class GeoDataRiverSurveyImporterSettingDialog;
}

class GeoDataRiverSurveyImporterSettingDialog : public QDialog
{
	Q_OBJECT

public:
	enum CenterPointSetting {
		cpMiddle,
		cpElevation,
		cpLowWaterWay
	};

	explicit GeoDataRiverSurveyImporterSettingDialog(QWidget* parent = nullptr);
	~GeoDataRiverSurveyImporterSettingDialog();

	void setWith4Points(bool with4points);
	void setAllNamesAreNumber(bool allNumber);

	CenterPointSetting centerPointSetting() const;
	bool reverseOrder() const;

private:
	Ui::GeoDataRiverSurveyImporterSettingDialog* ui;
};

#endif // GEODATARIVERSURVEYIMPORTERSETTINGDIALOG_H
