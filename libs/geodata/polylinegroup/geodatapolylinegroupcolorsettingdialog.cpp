#include "ui_geodatapolylinegroupcolorsettingdialog.h"

#include "geodatapolylinegroupcolorsettingdialog.h"

GeoDataPolyLineGroupColorSettingDialog::Setting::Setting() :
	CompositeContainer ({&color, &opacity, &mapping, &lineWidth}),
	color {"color"},
	opacity {},
	mapping {"mapping", Value},
	lineWidth {"lineWidth"}
{
	opacity = 50;
	lineWidth = 1;
}

GeoDataPolyLineGroupColorSettingDialog::Setting::Setting(const Setting& s) :
	Setting()
{
	CompositeContainer::copyValue(s);
}

GeoDataPolyLineGroupColorSettingDialog::Setting& GeoDataPolyLineGroupColorSettingDialog::Setting::operator=(const Setting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}

GeoDataPolyLineGroupColorSettingDialog::GeoDataPolyLineGroupColorSettingDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::GeoDataPolyLineGroupColorSettingDialog}
{
	ui->setupUi(this);
}

GeoDataPolyLineGroupColorSettingDialog::~GeoDataPolyLineGroupColorSettingDialog()
{
	delete ui;
}

void GeoDataPolyLineGroupColorSettingDialog::setIsReferenceInformation(bool isRef)
{
	if (! isRef) {return;}

	ui->arbitraryRadioButton->setChecked(true);
	ui->byValueRadioButton->setDisabled(true);
}

void GeoDataPolyLineGroupColorSettingDialog::setSetting(const Setting& setting)
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

	// line width
	ui->lineWidthSpinBox->setValue(setting.lineWidth);
}

GeoDataPolyLineGroupColorSettingDialog::Setting GeoDataPolyLineGroupColorSettingDialog::setting() const
{
	Setting ret;
	// mapping
	if (ui->byValueRadioButton->isChecked()) {ret.mapping = Value;}
	if (ui->arbitraryRadioButton->isChecked()) {ret.mapping = Arbitrary;}

	// opacity
	ret.opacity = ui->transparencyWidget->opacityPercent();

	// color
	ret.color = ui->colorEditWidget->color();

	// line width
	ret.lineWidth = ui->lineWidthSpinBox->value();

	return ret;
}
