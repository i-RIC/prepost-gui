#ifndef RAWDATARIVERSURVEYIMPORTERSETTINGDIALOG_H
#define RAWDATARIVERSURVEYIMPORTERSETTINGDIALOG_H

#include <QDialog>

namespace Ui
{
	class RawDataRiverSurveyImporterSettingDialog;
}

class RawDataRiverSurveyImporterSettingDialog : public QDialog
{
	Q_OBJECT

public:
	enum CenterPointSetting {
		cpMiddle,
		cpElevation
	};

	explicit RawDataRiverSurveyImporterSettingDialog(QWidget* parent = nullptr);
	~RawDataRiverSurveyImporterSettingDialog();

	CenterPointSetting centerPointSetting() const;
private:
	Ui::RawDataRiverSurveyImporterSettingDialog* ui;
};

#endif // RAWDATARIVERSURVEYIMPORTERSETTINGDIALOG_H
