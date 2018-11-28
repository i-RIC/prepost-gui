#include "ui_geodatapolylinecolorsimplesettingdialog.h"

#include "geodatapolylinecolorsimplesettingdialog.h"

GeoDataPolyLineColorSimpleSettingDialog::Setting::Setting() :
	CompositeContainer ({&color}),
	color {"color"}
{}

GeoDataPolyLineColorSimpleSettingDialog::Setting::Setting(const Setting& s) :
	Setting()
{
	CompositeContainer::copyValue(s);
}

GeoDataPolyLineColorSimpleSettingDialog::Setting& GeoDataPolyLineColorSimpleSettingDialog::Setting::operator=(const Setting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}

GeoDataPolyLineColorSimpleSettingDialog::GeoDataPolyLineColorSimpleSettingDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::GeoDataPolyLineColorSimpleSettingDialog}
{
	ui->setupUi(this);
}

GeoDataPolyLineColorSimpleSettingDialog::~GeoDataPolyLineColorSimpleSettingDialog()
{
	delete ui;
}

void GeoDataPolyLineColorSimpleSettingDialog::setSetting(const Setting& setting)
{
	// color
	ui->colorEditWidget->setColor(setting.color);
}

GeoDataPolyLineColorSimpleSettingDialog::Setting GeoDataPolyLineColorSimpleSettingDialog::setting() const
{
	Setting ret;

	// color
	ret.color = ui->colorEditWidget->color();

	return ret;
}
