#include "ui_geodatapolygoncolorsettingdialog.h"

#include "geodatapolygoncolorsettingdialog.h"

GeoDataPolygonColorSettingDialog::GeoDataPolygonColorSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GeoDataPolygonColorSettingDialog)
{
	ui->setupUi(this);
}

GeoDataPolygonColorSettingDialog::~GeoDataPolygonColorSettingDialog()
{
	delete ui;
}

void GeoDataPolygonColorSettingDialog::setMapping(Mapping m)
{
	switch (m) {
	case Value:
		ui->byValueRadioButton->click();
		break;
	case Arbitrary:
		ui->arbitraryRadioButton->click();
		break;
	}
}

void GeoDataPolygonColorSettingDialog::setOpacityPercent(int o)
{
	ui->transparencyWidget->setOpacity(o);
}

GeoDataPolygonColorSettingDialog::Mapping GeoDataPolygonColorSettingDialog::mapping()
{
	if (ui->byValueRadioButton->isChecked()) { return Value; }
	if (ui->arbitraryRadioButton->isChecked()) { return Arbitrary; }
	//default
	return Value;
}

void GeoDataPolygonColorSettingDialog::setColor(const QColor& color)
{
	ui->colorEditWidget->setColor(color);
}

QColor GeoDataPolygonColorSettingDialog::color()
{
	return ui->colorEditWidget->color();
}

int GeoDataPolygonColorSettingDialog::opacityPercent()
{
	return ui->transparencyWidget->opacity();
}

