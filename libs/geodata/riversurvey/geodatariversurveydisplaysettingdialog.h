#ifndef GEODATARIVERSURVEYDISPLAYSETTINGDIALOG_H
#define GEODATARIVERSURVEYDISPLAYSETTINGDIALOG_H

#include <QDialog>

namespace Ui
{
	class GeoDataRiverSurveyDisplaySettingDialog;
}

class QColor;

class GeoDataRiverSurveyDisplaySettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit GeoDataRiverSurveyDisplaySettingDialog(QWidget* parent = nullptr);
	~GeoDataRiverSurveyDisplaySettingDialog();
	void setColormapVisible(bool visible);
	void setOpacityPercent(int opacity);
	void setLinesVisible(bool visible);
	void setLineColor(const QColor& color);
	void setZScale(int scale);

	bool colormapVisible();
	int opacityPercent();
	bool linesVisible();
	QColor lineColor();
	int zScale();

private:
	Ui::GeoDataRiverSurveyDisplaySettingDialog* ui;
};

#endif // GEODATARIVERSURVEYDISPLAYSETTINGDIALOG_H
