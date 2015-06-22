#include "ui_geodatapointmaprepresentationdialog.h"

#include "geodatapointmaprepresentationdialog.h"

GeoDataPointmapRepresentationDialog::GeoDataPointmapRepresentationDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GeoDataPointmapRepresentationDialog)
{
	ui->setupUi(this);
	ui->transparencyWidget->setOpacity(100);
}

GeoDataPointmapRepresentationDialog::~GeoDataPointmapRepresentationDialog()
{
	delete ui;
}

void GeoDataPointmapRepresentationDialog::setRepresentation(Representation r)
{
	switch (r) {
	case Points:
		ui->pointsRadioButton->click();
		break;
	case Wireframe:
		ui->wireframeRadioButton->click();
		break;
	case Surface:
		ui->surfaceRadioButton->click();
		break;
	}
}

GeoDataPointmapRepresentationDialog::Representation GeoDataPointmapRepresentationDialog::representation()
{
	if (ui->pointsRadioButton->isChecked()) { return Points; }
	if (ui->wireframeRadioButton->isChecked()) { return Wireframe; }
	if (ui->surfaceRadioButton->isChecked()) { return Surface; }
	//default
	return Points;
}

void GeoDataPointmapRepresentationDialog::setOpacityPercent(int opacity)
{
	if (ui->surfaceRadioButton->isChecked()) {
		ui->transparencyWidget->setOpacity(opacity);
	}
}

int GeoDataPointmapRepresentationDialog::opacityPercent()
{
	return ui->transparencyWidget->opacity();
}

void GeoDataPointmapRepresentationDialog::setHideBreakLines(bool hide)
{
	ui->hideBreaklinesCheckBox->setChecked(hide);
}

void GeoDataPointmapRepresentationDialog::setPointSize(int size)
{
	ui->pointSizeSpinBox->setValue(size);
}

bool GeoDataPointmapRepresentationDialog::hideBreakLines()
{
	return ui->hideBreaklinesCheckBox->isChecked();
}

int GeoDataPointmapRepresentationDialog::pointSize()
{
	return ui->pointSizeSpinBox->value();
}
