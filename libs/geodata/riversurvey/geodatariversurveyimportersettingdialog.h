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
		cpFile,
	};

	explicit GeoDataRiverSurveyImporterSettingDialog(QWidget* parent = nullptr);
	~GeoDataRiverSurveyImporterSettingDialog();

	void setWith4Points(bool with4points);
	void setAllNamesAreNumber(bool allNumber);
	void setFileName(const QString& name);

	CenterPointSetting centerPointSetting() const;
	bool reverseOrder() const;
	QString csvFileName() const;

private:
	Ui::GeoDataRiverSurveyImporterSettingDialog* ui;
};

#endif // GEODATARIVERSURVEYIMPORTERSETTINGDIALOG_H
