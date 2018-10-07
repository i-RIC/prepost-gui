#include "ui_geodatapolylinecolorsettingdialog.h"

#include "geodatapolylinecolorsettingdialog.h"

GeoDataPolyLineColorSettingDialog::Setting::Setting() :
	CompositeContainer ({&color, &mapping}),
	color {"color"},
	mapping {"mapping", Value}
{}

GeoDataPolyLineColorSettingDialog::Setting::Setting(const Setting& s) :
	Setting()
{
	CompositeContainer::copyValue(s);
}

GeoDataPolyLineColorSettingDialog::Setting& GeoDataPolyLineColorSettingDialog::Setting::operator=(const Setting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}

GeoDataPolyLineColorSettingDialog::GeoDataPolyLineColorSettingDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::GeoDataPolyLineColorSettingDialog}
{
	ui->setupUi(this);
}

GeoDataPolyLineColorSettingDialog::~GeoDataPolyLineColorSettingDialog()
{
	delete ui;
}

void GeoDataPolyLineColorSettingDialog::setSetting(const Setting& setting)
{
	// mapping
	if (setting.mapping == Value) {
		ui->byValueRadioButton->setChecked(true);
	} else if (setting.mapping == Arbitrary) {
		ui->arbitraryRadioButton->setChecked(true);
	}

	// color
	ui->colorEditWidget->setColor(setting.color);
}

GeoDataPolyLineColorSettingDialog::Setting GeoDataPolyLineColorSettingDialog::setting() const
{
	Setting ret;
	// mapping
	if (ui->byValueRadioButton->isChecked()) {ret.mapping = Value;}
	if (ui->arbitraryRadioButton->isChecked()) {ret.mapping = Arbitrary;}

	// color
	ret.color = ui->colorEditWidget->color();

	return ret;
}
