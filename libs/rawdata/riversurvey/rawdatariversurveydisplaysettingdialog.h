#ifndef RAWDATARIVERSURVEYDISPLAYSETTINGDIALOG_H
#define RAWDATARIVERSURVEYDISPLAYSETTINGDIALOG_H

#include <QDialog>

namespace Ui {
	class RawDataRiverSurveyDisplaySettingDialog;
}

class QColor;

class RawDataRiverSurveyDisplaySettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit RawDataRiverSurveyDisplaySettingDialog(QWidget *parent = 0);
	~RawDataRiverSurveyDisplaySettingDialog();
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
	Ui::RawDataRiverSurveyDisplaySettingDialog *ui;
};

#endif // RAWDATARIVERSURVEYDISPLAYSETTINGDIALOG_H
