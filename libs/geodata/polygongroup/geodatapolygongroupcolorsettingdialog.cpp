#include "ui_geodatapolygongroupcolorsettingdialog.h"

#include "geodatapolygongroupcolorsettingdialog.h"

GeoDataPolygonGroupColorSettingDialog::Setting::Setting() :
	CompositeContainer ({&color, &opacity, &mapping, &lineWidth}),
	color {"color"},
	opacity {},
	mapping {"mapping", Value},
	lineWidth {"lineWidth"}
{
	opacity = 50;
	lineWidth = 1;
}

GeoDataPolygonGroupColorSettingDialog::Setting::Setting(const Setting& s) :
	Setting()
{
	CompositeContainer::copyValue(s);
}

GeoDataPolygonGroupColorSettingDialog::Setting& GeoDataPolygonGroupColorSettingDialog::Setting::operator=(const Setting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}

GeoDataPolygonGroupColorSettingDialog::GeoDataPolygonGroupColorSettingDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::GeoDataPolygonGroupColorSettingDialog}
{
	ui->setupUi(this);
}

GeoDataPolygonGroupColorSettingDialog::~GeoDataPolygonGroupColorSettingDialog()
{
	delete ui;
}

void GeoDataPolygonGroupColorSettingDialog::setIsReferenceInformation(bool isRef)
{
	if (! isRef) {return;}

	ui->arbitraryRadioButton->setChecked(true);
	ui->byValueRadioButton->setDisabled(true);
}

void GeoDataPolygonGroupColorSettingDialog::setSetting(const Setting& setting)
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

GeoDataPolygonGroupColorSettingDialog::Setting GeoDataPolygonGroupColorSettingDialog::setting() const
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
