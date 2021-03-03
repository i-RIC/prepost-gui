#include "crosssectionwindowdisplaysettingdialog.h"
#include "ui_crosssectionwindowdisplaysettingdialog.h"

#include <QFontDialog>

CrossSectionWindowDisplaySettingDialog::CrossSectionWindowDisplaySettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::CrossSectionWindowDisplaySettingDialog)
{
	ui->setupUi(this);
	connect(ui->distanceMarkersFontButton, SIGNAL(clicked()), this, SLOT(editScalesFont()));
	connect(ui->aspectRatioFontButton, SIGNAL(clicked()), this, SLOT(editAspectRatioFont()));
	connect(ui->HWMsFontButton, SIGNAL(clicked()), this, SLOT(editHWMsFont()));
}

CrossSectionWindowDisplaySettingDialog::~CrossSectionWindowDisplaySettingDialog()
{
	delete ui;
}

CrossSectionWindowDisplaySetting CrossSectionWindowDisplaySettingDialog::crossSectionWindowDisplaySetting() const
{
	auto s = m_crossSectionWindowDisplaySetting;
	s.XSColor = ui->crossSectionPointColorWidget->color();
	s.WSEColor = ui->WSEColorWidget->color();
	s.LeftHWMColor = ui->LeftHWMColorWidget->color();
	s.RightHWMColor = ui->RightHWMColorWidget->color();

	return s;
}

ChartGraphicsViewDisplaySetting CrossSectionWindowDisplaySettingDialog::chartGraphicsViewDisplaySetting() const
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

void CrossSectionWindowDisplaySettingDialog::setSettings(const CrossSectionWindowDisplaySetting& csds, const ChartGraphicsViewDisplaySetting& chartds)
{
	m_crossSectionWindowDisplaySetting = csds;
	m_chartGraphicsViewDisplaySetting = chartds;

	ui->crossSectionPointColorWidget->setColor(csds.XSColor);
	ui->WSEColorWidget->setColor(csds.WSEColor);
	ui->LeftHWMColorWidget->setColor(csds.LeftHWMColor);
	ui->RightHWMColorWidget->setColor(csds.RightHWMColor);

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

void CrossSectionWindowDisplaySettingDialog::editScalesFont()
{
	bool ok;
	auto newFont = QFontDialog::getFont(&ok, m_chartGraphicsViewDisplaySetting.distanceMarkersFont, this, tr("Scales font"));
	if (! ok) {return;}

	m_chartGraphicsViewDisplaySetting.distanceMarkersFont = newFont;
}

void CrossSectionWindowDisplaySettingDialog::editAspectRatioFont()
{
	bool ok;
	auto newFont = QFontDialog::getFont(&ok, m_chartGraphicsViewDisplaySetting.aspectRatioFont, this, tr("Scales font"));
	if (! ok) {return;}

	m_chartGraphicsViewDisplaySetting.aspectRatioFont = newFont;
}

void CrossSectionWindowDisplaySettingDialog::editHWMsFont()
{
	bool ok;
	auto newFont = QFontDialog::getFont(&ok, m_crossSectionWindowDisplaySetting.HWMFont, this, tr("Scales font"));
	if (! ok) {return;}

	m_crossSectionWindowDisplaySetting.HWMFont = newFont;
}
