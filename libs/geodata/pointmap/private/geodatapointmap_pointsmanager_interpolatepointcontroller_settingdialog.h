#ifndef GEODATAPOINTMAP_POINTSMANAGER_INTERPOLATEPOINTCONTROLLER_SETTINGDIALOG_H
#define GEODATAPOINTMAP_POINTSMANAGER_INTERPOLATEPOINTCONTROLLER_SETTINGDIALOG_H

#include "geodatapointmap_pointsmanager_interpolatepointcontroller.h"

#include <QDialog>

namespace Ui {
class GeoDataPointmap_PointsManager_InterpolatePointController_SettingDialog;
}

class GeoDataPointmap::PointsManager::InterpolatePointController::SettingDialog : public QDialog
{
	Q_OBJECT

public:
	explicit SettingDialog(QWidget *parent = nullptr);
	~SettingDialog();

	void setLineLength(double length);
	double increment() const;

public slots:
	void accept() override;

private:
	double m_lineLength;
	Ui::GeoDataPointmap_PointsManager_InterpolatePointController_SettingDialog *ui;
};

#endif // GEODATAPOINTMAP_POINTSMANAGER_INTERPOLATEPOINTCONTROLLER_SETTINGDIALOG_H
