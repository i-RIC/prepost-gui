#include "geodatapointmap_pointsmanager_interpolatepointcontroller_settingdialog.h"
#include "ui_geodatapointmap_pointsmanager_interpolatepointcontroller_settingdialog.h"

#include <QMessageBox>

GeoDataPointmap::PointsManager::InterpolatePointController::SettingDialog::SettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GeoDataPointmap_PointsManager_InterpolatePointController_SettingDialog)
{
	ui->setupUi(this);
}

GeoDataPointmap::PointsManager::InterpolatePointController::SettingDialog::~SettingDialog()
{
	delete ui;
}

void GeoDataPointmap::PointsManager::InterpolatePointController::SettingDialog::setLineLength(double length)
{
	m_lineLength = length;
	ui->lineLengthValueLabel->setText(QString::number(length, 'g', 3));
}

double GeoDataPointmap::PointsManager::InterpolatePointController::SettingDialog::increment() const
{
	return ui->incrementEditWidget->value();
}

void GeoDataPointmap::PointsManager::InterpolatePointController::SettingDialog::accept()
{
	auto inc = increment();
	if (inc <= 0 || inc >= m_lineLength) {
		QMessageBox::warning(this, tr("Warning"), tr("Interpolation increment must be greater 0 and less than interpolation line length"));
		return;
	}

	QDialog::accept();
}
