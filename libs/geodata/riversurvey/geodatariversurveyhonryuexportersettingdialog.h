#ifndef GEODATARIVERSURVEYHONRYUEXPORTERSETTINGDIALOG_H
#define GEODATARIVERSURVEYHONRYUEXPORTERSETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class GeoDataRiverSurveyHonryuExporterSettingDialog;
}

class GeoDataRiverSurveyHonryuExporterSettingDialog : public QDialog
{
	Q_OBJECT

public:
	enum Separator {
		Comma,
		Space,
		Tab,
	};

	enum Columns {
		Name_LongDistance_Distance_Elevation,
		LongDistance_Name_Distance_Elevation,
		Distance_Elevation_Name_LongDistance,
		Distance_Elevation_LongDistance_Name
	};

	enum LongitudinalDistance {
		From_Previouos,
		From_First
	};

	explicit GeoDataRiverSurveyHonryuExporterSettingDialog(QWidget *parent = nullptr);
	~GeoDataRiverSurveyHonryuExporterSettingDialog();

	void setCsvMode(bool csvMode);

	Separator separator() const;
	bool addHeader() const;
	LongitudinalDistance longitudinalDistance() const;
	Columns columns() const;

private:
	Ui::GeoDataRiverSurveyHonryuExporterSettingDialog *ui;
};

#endif // GEODATARIVERSURVEYHONRYUEXPORTERSETTINGDIALOG_H
