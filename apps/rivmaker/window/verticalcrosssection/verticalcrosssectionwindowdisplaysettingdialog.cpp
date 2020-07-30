#include "verticalcrosssectionwindowdisplaysettingdialog.h"
#include "ui_verticalcrosssectionwindowdisplaysettingdialog.h"

#include <QFontDialog>

VerticalCrossSectionWindowDisplaySettingDialog::VerticalCrossSectionWindowDisplaySettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::VerticalCrossSectionWindowDisplaySettingDialog)
{
	ui->setupUi(this);
	connect(ui->distanceMarkersFontButton, SIGNAL(clicked()), this, SLOT(editScalesFont()));
	connect(ui->aspectRatioFontButton, SIGNAL(clicked()), this, SLOT(editAspectRatioFont()));
}

VerticalCrossSectionWindowDisplaySettingDialog::~VerticalCrossSectionWindowDisplaySettingDialog()
{
	delete ui;
}

ChartGraphicsViewDisplaySetting VerticalCrossSectionWindowDisplaySettingDialog::chartGraphicsViewDisplaySetting() const
{
	auto s = m_chartGraphicsViewDisplaySetting;

	s.bgColor = ui->BackgroundColorWidget->color();

	if (ui->bgLinesRadioButton->isChecked()) {
		s.bgGridType = ChartGraphicsViewDisplaySetting::BackgroundGridType::Lines;
	} else {
		s.bgGridType = ChartGraphicsViewDisplaySetting::BackgroundGridType::Dots;
	}
	s.bgGridColor = ui->gridColorWidget->color();

	s.bgHScaleAuto = ui->horizontalAutoCheckBox->isChecked();
	s.bgHScaleInterval = ui->horizontalScaleSpinBox->value();
	s.bgHSubScaleInterval = ui->horizontalSubScaleSpinBox->value();

	s.bgVScaleAuto = ui->verticalAutoCheckBox->isChecked();
	s.bgVScaleInterval = ui->verticalScaleSpinBox->value();
	s.bgVSubScaleInterval = ui->verticalSubScaleSpinBox->value();

	s.aspectRatioColor = ui->aspectRatioColorWidget->color();
	s.distanceMarkersColor = ui->distanceMarkersColorWidget->color();

	return s;
}

void VerticalCrossSectionWindowDisplaySettingDialog::setSettings(const ChartGraphicsViewDisplaySetting& chartds)
{
	m_chartGraphicsViewDisplaySetting = chartds;

	ui->BackgroundColorWidget->setColor(chartds.bgColor);

	if (chartds.bgGridType == ChartGraphicsViewDisplaySetting::BackgroundGridType::Lines) {
		ui->bgLinesRadioButton->setChecked(true);
	} else {
		ui->bgDotsRadioButton->setChecked(true);
	}
	ui->gridColorWidget->setColor(chartds.bgGridColor);

	ui->horizontalAutoCheckBox->setChecked(chartds.bgHScaleAuto);
	ui->horizontalScaleSpinBox->setValue(chartds.bgHScaleInterval);
	ui->horizontalSubScaleSpinBox->setValue(chartds.bgHSubScaleInterval);

	ui->verticalAutoCheckBox->setChecked(chartds.bgVScaleAuto);
	ui->verticalScaleSpinBox->setValue(chartds.bgVScaleInterval);
	ui->verticalSubScaleSpinBox->setValue(chartds.bgVSubScaleInterval);

	ui->aspectRatioColorWidget->setColor(chartds.aspectRatioColor);
	ui->distanceMarkersColorWidget->setColor(chartds.distanceMarkersColor);
}

void VerticalCrossSectionWindowDisplaySettingDialog::editScalesFont()
{
	bool ok;
	auto newFont = QFontDialog::getFont(&ok, m_chartGraphicsViewDisplaySetting.distanceMarkersFont, this, tr("Scales font"));
	if (! ok) {return;}

	m_chartGraphicsViewDisplaySetting.distanceMarkersFont = newFont;
}

void VerticalCrossSectionWindowDisplaySettingDialog::editAspectRatioFont()
{
	bool ok;
	auto newFont = QFontDialog::getFont(&ok, m_chartGraphicsViewDisplaySetting.aspectRatioFont, this, tr("Scales font"));
	if (! ok) {return;}

	m_chartGraphicsViewDisplaySetting.aspectRatioFont = newFont;
}
