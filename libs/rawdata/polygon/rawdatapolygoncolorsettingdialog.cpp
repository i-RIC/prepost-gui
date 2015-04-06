#include "rawdatapolygoncolorsettingdialog.h"
#include "ui_rawdatapolygoncolorsettingdialog.h"

RawDataPolygonColorSettingDialog::RawDataPolygonColorSettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::RawDataPolygonColorSettingDialog)
{
	ui->setupUi(this);
}

RawDataPolygonColorSettingDialog::~RawDataPolygonColorSettingDialog()
{
	delete ui;
}

void RawDataPolygonColorSettingDialog::setMapping(Mapping m)
{
	switch (m)
	{
	case Value:
		ui->byValueRadioButton->click();
		break;
	case Arbitrary:
		ui->arbitraryRadioButton->click();
		break;
	}
}

void RawDataPolygonColorSettingDialog::setOpacityPercent(int o)
{
	ui->transparencyWidget->setOpacity(o);
}

RawDataPolygonColorSettingDialog::Mapping RawDataPolygonColorSettingDialog::mapping()
{
	if (ui->byValueRadioButton->isChecked()) return Value;
	if (ui->arbitraryRadioButton->isChecked()) return Arbitrary;
	//default
	return Value;
}

void RawDataPolygonColorSettingDialog::setColor(const QColor &color)
{
	ui->colorEditWidget->setColor(color);
}

QColor RawDataPolygonColorSettingDialog::color()
{
	return ui->colorEditWidget->color();
}

int RawDataPolygonColorSettingDialog::opacityPercent()
{
	return ui->transparencyWidget->opacity();
}

