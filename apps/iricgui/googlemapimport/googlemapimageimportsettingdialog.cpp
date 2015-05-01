#include "googlemapimageimportsettingdialog.h"
#include "ui_googlemapimageimportsettingdialog.h"

GoogleMapImageImportSettingDialog::GoogleMapImageImportSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GoogleMapImageImportSettingDialog)
{
	ui->setupUi(this);
}

GoogleMapImageImportSettingDialog::~GoogleMapImageImportSettingDialog()
{
	delete ui;
}

void GoogleMapImageImportSettingDialog::setZoom(int zoom)
{
	ui->zoomSpinBox->setValue(zoom);
}

void GoogleMapImageImportSettingDialog::setZoomMax(int zoomMax)
{
	ui->zoomSpinBox->setMaximum(zoomMax);
}

int GoogleMapImageImportSettingDialog::zoom() const
{
	return ui->zoomSpinBox->value();
}

QString GoogleMapImageImportSettingDialog::maptype() const
{
	switch (ui->mapTypeComboBox->currentIndex()) {
	case 0:
		return "satellite";
		break;
	case 1:
		return "roadmap";
		break;
	case 2:
		return "terrain";
		break;
	case 3:
		return "hybrid";
		break;
	default:
		return "satellite";
	}
}
