#include "geodatariversurveycrosssectiondisplaysettingdialog.h"
#include "ui_geodatariversurveycrosssectiondisplaysettingdialog.h"

#include <QFontDialog>

GeoDataRiverSurveyCrossSectionDisplaySettingDialog::GeoDataRiverSurveyCrossSectionDisplaySettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GeoDataRiverSurveyCrossSectionDisplaySettingDialog)
{
	ui->setupUi(this);

	connect(ui->aspectRatioFontButton, SIGNAL(clicked(bool)), this, SLOT(editAspectRatioFont()));
	connect(ui->distanceMarkersFontButton, SIGNAL(clicked(bool)), this, SLOT(editDistanceMarkersFont()));
	connect(ui->lbBankMakersFontButton, SIGNAL(clicked(bool)), this, SLOT(editLbBankMarkersFont()));
}

GeoDataRiverSurveyCrossSectionDisplaySettingDialog::~GeoDataRiverSurveyCrossSectionDisplaySettingDialog()
{
	delete ui;
}

GeoDataRiverSurveyCrossSectionDisplaySetting GeoDataRiverSurveyCrossSectionDisplaySettingDialog::setting()
{
	if (ui->bgLinesRadioButton->isChecked()) {
		m_setting.bgGridType = GeoDataRiverSurveyCrossSectionDisplaySetting::BackgroundGridType::Lines;
	} else if (ui->bgDotsRadioButton->isChecked()) {
		m_setting.bgGridType = GeoDataRiverSurveyCrossSectionDisplaySetting::BackgroundGridType::Dots;
	}
	m_setting.bgGridColor = ui->gridColorWidget->color();

	m_setting.bgHScaleAuto = ui->horizontalAutoCheckBox->isChecked();
	m_setting.bgHScaleInterval = ui->horizontalScaleSpinBox->value();
	m_setting.bgHSubScaleInterval = ui->horizontalSubScaleSpinBox->value();

	m_setting.bgVScaleAuto = ui->verticalAutoCheckBox->isChecked();
	m_setting.bgVScaleInterval = ui->verticalScaleSpinBox->value();
	m_setting.bgVSubScaleInterval = ui->verticalSubScaleSpinBox->value();

	m_setting.aspectRatioColor = ui->aspectRatioColorWidget->color();
	m_setting.distanceMarkersColor = ui->distanceMarkersColorWidget->color();
	m_setting.lbBankMarkersColor = ui->lbBankMakersColorWidget->color();

	return m_setting;
}

void GeoDataRiverSurveyCrossSectionDisplaySettingDialog::setSetting(const GeoDataRiverSurveyCrossSectionDisplaySetting& setting)
{
	m_setting = setting;

	if (setting.bgGridType == GeoDataRiverSurveyCrossSectionDisplaySetting::BackgroundGridType::Lines) {
		ui->bgLinesRadioButton->setChecked(true);
	} else {
		ui->bgDotsRadioButton->setChecked(true);
	}
	ui->gridColorWidget->setColor(setting.bgGridColor);

	ui->horizontalAutoCheckBox->setChecked(setting.bgHScaleAuto);
	ui->horizontalScaleSpinBox->setValue(setting.bgHScaleInterval);
	ui->horizontalSubScaleSpinBox->setValue(setting.bgHSubScaleInterval);

	ui->verticalAutoCheckBox->setChecked(setting.bgVScaleAuto);
	ui->verticalScaleSpinBox->setValue(setting.bgVScaleInterval);
	ui->verticalSubScaleSpinBox->setValue(setting.bgVSubScaleInterval);

	ui->aspectRatioColorWidget->setColor(m_setting.aspectRatioColor);
	ui->distanceMarkersColorWidget->setColor(m_setting.distanceMarkersColor);
	ui->lbBankMakersColorWidget->setColor(m_setting.lbBankMarkersColor);
}

void GeoDataRiverSurveyCrossSectionDisplaySettingDialog::editAspectRatioFont()
{
	bool ok;
	auto newFont = QFontDialog::getFont(&ok, m_setting.aspectRatioFont, this, tr("Left/right bank markers font"));
	if (! ok) {return;}

	m_setting.aspectRatioFont = newFont;
}

void GeoDataRiverSurveyCrossSectionDisplaySettingDialog::editDistanceMarkersFont()
{
	bool ok;
	auto newFont = QFontDialog::getFont(&ok, m_setting.distanceMarkersFont, this, tr("Left/right bank markers font"));
	if (! ok) {return;}

	m_setting.distanceMarkersFont = newFont;
}

void GeoDataRiverSurveyCrossSectionDisplaySettingDialog::editLbBankMarkersFont()
{
	bool ok;
	auto newFont = QFontDialog::getFont(&ok, m_setting.lbBankMarkersFont, this, tr("Left/right bank markers font"));
	if (! ok) {return;}

	m_setting.lbBankMarkersFont = newFont;
}
