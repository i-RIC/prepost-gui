#include "ui_geodatanetcdfcolorsettingdialog.h"

#include "geodatanetcdfcolorsettingdialog.h"

GeoDataNetcdfColorSettingDialog::Setting::Setting() :
	CompositeContainer ({&color, &opacity, &mapping}),
	color {"color"},
	opacity {},
	mapping {"mapping", Value}
{
	opacity = 50;
}

GeoDataNetcdfColorSettingDialog::Setting::Setting(const Setting& s) :
	Setting()
{
	CompositeContainer::copyValue(s);
}

GeoDataNetcdfColorSettingDialog::Setting& GeoDataNetcdfColorSettingDialog::Setting::operator=(const Setting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}

GeoDataNetcdfColorSettingDialog::GeoDataNetcdfColorSettingDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::GeoDataNetcdfColorSettingDialog}
{
	ui->setupUi(this);
}

GeoDataNetcdfColorSettingDialog::~GeoDataNetcdfColorSettingDialog()
{
	delete ui;
}

void GeoDataNetcdfColorSettingDialog::setIsReferenceInformation(bool isRef)
{
	if (! isRef) {return;}

	ui->arbitraryRadioButton->setChecked(true);
	ui->byValueRadioButton->setDisabled(true);
}

void GeoDataNetcdfColorSettingDialog::setSetting(const Setting& setting)
{
	// mapping
	if (setting.mapping == Value) {
		ui->byValueRadioButton->setChecked(true);
	} else if (setting.mapping == Arbitrary) {
		ui->arbitraryRadioButton->setChecked(true);
	}

	// opacity
	ui->transparencyWidget->setOpacityPercent(setting.opacity);

	// color
	ui->colorEditWidget->setColor(setting.color);
}

GeoDataNetcdfColorSettingDialog::Setting GeoDataNetcdfColorSettingDialog::setting() const
{
	Setting ret;
	// mapping
	if (ui->byValueRadioButton->isChecked()) {ret.mapping = Value;}
	if (ui->arbitraryRadioButton->isChecked()) {ret.mapping = Arbitrary;}

	// opacity
	ret.opacity = ui->transparencyWidget->opacityPercent();

	// color
	ret.color = ui->colorEditWidget->color();

	return ret;
}
