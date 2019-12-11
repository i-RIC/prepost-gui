#ifndef GEODATARIVERSURVEYCROSSSECTIONDISPLAYSETTINGDIALOG_H
#define GEODATARIVERSURVEYCROSSSECTIONDISPLAYSETTINGDIALOG_H

#include "geodatariversurveycrosssectiondisplaysetting.h"

#include <QDialog>

namespace Ui {
class GeoDataRiverSurveyCrossSectionDisplaySettingDialog;
}

class GeoDataRiverSurveyCrossSectionDisplaySetting;

class GeoDataRiverSurveyCrossSectionDisplaySettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataRiverSurveyCrossSectionDisplaySettingDialog(QWidget *parent = 0);
	~GeoDataRiverSurveyCrossSectionDisplaySettingDialog();

	GeoDataRiverSurveyCrossSectionDisplaySetting setting();
	void setSetting(const GeoDataRiverSurveyCrossSectionDisplaySetting& setting);

private slots:
	void editAspectRatioFont();
	void editDistanceMarkersFont();
	void editLbBankMarkersFont();

private:
	GeoDataRiverSurveyCrossSectionDisplaySetting m_setting;
	Ui::GeoDataRiverSurveyCrossSectionDisplaySettingDialog *ui;
};

#endif // GEODATARIVERSURVEYCROSSSECTIONDISPLAYSETTINGDIALOG_H
