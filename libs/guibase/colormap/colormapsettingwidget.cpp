#include "ui_colormapsettingwidget.h"

#include "colormapcustomsettingdialog.h"
#include "colormapsettingwidget.h"

#include <QDomNode>
#include <QXmlStreamWriter>


ColorMapSettingWidget::ColorMapSettingWidget(QWidget* parent) :
	QWidget(parent),
	ui(new Ui::ColorMapSettingWidget)
{
	ui->setupUi(this);
	connect(ui->customSettingButton, SIGNAL(clicked()), this, SLOT(openCustomSettingDialog()));
}

ColorMapSettingWidget::~ColorMapSettingWidget()
{
	delete ui;
}

void ColorMapSettingWidget::setColorMap(ColorMap colorMap)
{
	switch (colorMap) {
	case Rainbow:
		ui->rainbowRadioButton->setChecked(true);
		break;
	case Grayscale:
		ui->grayscaleRadioButton->setChecked(true);
		break;
	case Custom:
		ui->customRadioButton->setChecked(true);
		break;
	}
}

ColorMapSettingWidget::ColorMap ColorMapSettingWidget::colorMap() const
{
	if (ui->rainbowRadioButton->isChecked()) {return Rainbow;}
	if (ui->grayscaleRadioButton->isChecked()) {return Grayscale;}
	if (ui->customRadioButton->isChecked()) {return Custom;}

	// default
	return Rainbow;
}

const ColorMapCustomSetting& ColorMapSettingWidget::customSetting() const
{
	return m_customSetting;
}

void ColorMapSettingWidget::setCustomSetting(const ColorMapCustomSetting& setting)
{
	m_customSetting = setting;
}

void ColorMapSettingWidget::disableCustom()
{
	ui->customRadioButton->setDisabled(true);
	if (ui->customRadioButton->isChecked()) {
		ui->rainbowRadioButton->setChecked(true);
	}
}

void ColorMapSettingWidget::openCustomSettingDialog()
{
	ColorMapCustomSettingDialog dialog(this);
	dialog.setCustomSetting(m_customSetting);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	m_customSetting = dialog.customSetting();
}
