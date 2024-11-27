#include "ui_geodatagdalcolorsettingdialog.h"

#include "geodatagdalcolorsettingdialog.h"

GeoDataGdalColorSettingDialog::Setting::Setting() :
	CompositeContainer ({&color, &opacity, &mapping}),
	color {"color"},
	opacity {},
	mapping {"mapping", Value}
{
	opacity = 50;
}

GeoDataGdalColorSettingDialog::Setting::Setting(const Setting& s) :
	Setting()
{
	CompositeContainer::copyValue(s);
}

GeoDataGdalColorSettingDialog::Setting& GeoDataGdalColorSettingDialog::Setting::operator=(const Setting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}

GeoDataGdalColorSettingDialog::GeoDataGdalColorSettingDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::GeoDataGdalColorSettingDialog}
{
	ui->setupUi(this);
}

GeoDataGdalColorSettingDialog::~GeoDataGdalColorSettingDialog()
{
	delete ui;
}

void GeoDataGdalColorSettingDialog::setIsReferenceInformation(bool isRef)
{
	if (! isRef) {return;}

	ui->arbitraryRadioButton->setChecked(true);
	ui->byValueRadioButton->setDisabled(true);
}

void GeoDataGdalColorSettingDialog::setSetting(const Setting& setting)
{
	// mapping
	if (setting.mapping == Value) {
		ui->byValueRadioButton->setChecked(true);
	} else if (setting.mapping == Arbitrary) {
		ui->arbitraryRadioButton->setChecked(true);
	}

	// opacity
	ui->transparencyWidget->setOpacity(setting.opacity);

	// color
	ui->colorEditWidget->setColor(setting.color);
}

GeoDataGdalColorSettingDialog::Setting GeoDataGdalColorSettingDialog::setting() const
{
	Setting ret;
	// mapping
	if (ui->byValueRadioButton->isChecked()) {ret.mapping = Value;}
	if (ui->arbitraryRadioButton->isChecked()) {ret.mapping = Arbitrary;}

	// opacity
	ret.opacity = ui->transparencyWidget->opacity();

	// color
	ret.color = ui->colorEditWidget->color();

	return ret;
}
