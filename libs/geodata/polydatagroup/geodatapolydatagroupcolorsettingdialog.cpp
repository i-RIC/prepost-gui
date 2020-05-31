#include "ui_geodatapolydatagroupcolorsettingdialog.h"

#include "geodatapolydatagroupcolorsettingdialog.h"

GeoDataPolyDataGroupColorSettingDialog::Setting::Setting() :
	CompositeContainer ({&color, &opacity, &mapping}),
	color {"color"},
	opacity {},
	mapping {"mapping", Value}
{
	opacity = 50;
}

GeoDataPolyDataGroupColorSettingDialog::Setting::Setting(const Setting& s) :
	Setting()
{
	CompositeContainer::copyValue(s);
}

GeoDataPolyDataGroupColorSettingDialog::Setting& GeoDataPolyDataGroupColorSettingDialog::Setting::operator=(const Setting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}

GeoDataPolyDataGroupColorSettingDialog::GeoDataPolyDataGroupColorSettingDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::GeoDataPolyDataGroupColorSettingDialog}
{
	ui->setupUi(this);
}

GeoDataPolyDataGroupColorSettingDialog::~GeoDataPolyDataGroupColorSettingDialog()
{
	delete ui;
}

void GeoDataPolyDataGroupColorSettingDialog::setIsReferenceInformation(bool isRef)
{
	if (! isRef) {return;}

	ui->arbitraryRadioButton->setChecked(true);
	ui->byValueRadioButton->setDisabled(true);
}

void GeoDataPolyDataGroupColorSettingDialog::setSetting(const Setting& setting)
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

GeoDataPolyDataGroupColorSettingDialog::Setting GeoDataPolyDataGroupColorSettingDialog::setting() const
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
