#include "ui_geodatapointcolorsimplesettingdialog.h"

#include "geodatapointcolorsimplesettingdialog.h"

GeoDataPointColorSimpleSettingDialog::Setting::Setting() :
	CompositeContainer ({&color}),
	color {"color"}
{
	color = Qt::black;
}

GeoDataPointColorSimpleSettingDialog::Setting::Setting(const Setting& s) :
	Setting()
{
	CompositeContainer::copyValue(s);
}

GeoDataPointColorSimpleSettingDialog::Setting& GeoDataPointColorSimpleSettingDialog::Setting::operator=(const Setting& s)
{
	CompositeContainer::copyValue(s);
	return *this;
}

GeoDataPointColorSimpleSettingDialog::GeoDataPointColorSimpleSettingDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::GeoDataPointColorSimpleSettingDialog}
{
	ui->setupUi(this);
}

GeoDataPointColorSimpleSettingDialog::~GeoDataPointColorSimpleSettingDialog()
{
	delete ui;
}

void GeoDataPointColorSimpleSettingDialog::setSetting(const Setting& setting)
{
	// color
	ui->colorEditWidget->setColor(setting.color);
}

GeoDataPointColorSimpleSettingDialog::Setting GeoDataPointColorSimpleSettingDialog::setting() const
{
	Setting ret;

	// color
	ret.color = ui->colorEditWidget->color();

	return ret;
}
