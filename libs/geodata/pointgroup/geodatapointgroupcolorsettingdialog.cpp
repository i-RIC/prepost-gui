#include "ui_geodatapointgroupcolorsettingdialog.h"

#include "geodatapointgroupcolorsettingdialog.h"

GeoDataPointGroupColorSettingDialog::Setting::Setting() :
	CompositeContainer ({&color, &opacity, &mapping, &pointSize}),
	color {"color"},
	opacity {},
	mapping {"mapping", Value},
	pointSize {"pointSize"}

{
	opacity = 50;
	pointSize = 3;
}

GeoDataPointGroupColorSettingDialog::Setting::Setting(const Setting& s) :
	Setting()
{
	CompositeContainer::copyValue(s);
}

GeoDataPointGroupColorSettingDialog::Setting& GeoDataPointGroupColorSettingDialog::Setting::operator=(const Setting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}

GeoDataPointGroupColorSettingDialog::GeoDataPointGroupColorSettingDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::GeoDataPointGroupColorSettingDialog}
{
	ui->setupUi(this);
}

GeoDataPointGroupColorSettingDialog::~GeoDataPointGroupColorSettingDialog()
{
	delete ui;
}

void GeoDataPointGroupColorSettingDialog::setIsReferenceInformation(bool isRef)
{
	if (! isRef) {return;}

	ui->arbitraryRadioButton->setChecked(true);
	ui->byValueRadioButton->setDisabled(true);
}

void GeoDataPointGroupColorSettingDialog::setSetting(const Setting& setting)
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

	// point size
	ui->pointSizeSpinBox->setValue(setting.pointSize);
}

GeoDataPointGroupColorSettingDialog::Setting GeoDataPointGroupColorSettingDialog::setting() const
{
	Setting ret;
	// mapping
	if (ui->byValueRadioButton->isChecked()) {ret.mapping = Value;}
	if (ui->arbitraryRadioButton->isChecked()) {ret.mapping = Arbitrary;}

	// opacity
	ret.opacity = ui->transparencyWidget->opacityPercent();

	// color
	ret.color = ui->colorEditWidget->color();

	// point size
	ret.pointSize = ui->pointSizeSpinBox->value();

	return ret;
}
