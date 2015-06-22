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
		cpElevation
	};

	explicit GeoDataRiverSurveyImporterSettingDialog(QWidget* parent = nullptr);
	~GeoDataRiverSurveyImporterSettingDialog();

	CenterPointSetting centerPointSetting() const;

private:
	Ui::GeoDataRiverSurveyImporterSettingDialog* ui;
};

#endif // GEODATARIVERSURVEYIMPORTERSETTINGDIALOG_H
