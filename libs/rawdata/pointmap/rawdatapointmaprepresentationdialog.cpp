#include "rawdatapointmaprepresentationdialog.h"
#include "ui_rawdatapointmaprepresentationdialog.h"

RawDataPointmapRepresentationDialog::RawDataPointmapRepresentationDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::RawDataPointmapRepresentationDialog)
{
	ui->setupUi(this);
	ui->transparencyWidget->setOpacity(100);
}

RawDataPointmapRepresentationDialog::~RawDataPointmapRepresentationDialog()
{
	delete ui;
}

void RawDataPointmapRepresentationDialog::setRepresentation(Representation r)
{
	switch (r)
	{
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

RawDataPointmapRepresentationDialog::Representation RawDataPointmapRepresentationDialog::representation()
{
	if (ui->pointsRadioButton->isChecked()) return Points;
	if (ui->wireframeRadioButton->isChecked()) return Wireframe;
	if (ui->surfaceRadioButton->isChecked()) return Surface;
	//default
	return Points;
}

void RawDataPointmapRepresentationDialog::setOpacityPercent(int opacity)
{
	if (ui->surfaceRadioButton->isChecked()){
		ui->transparencyWidget->setOpacity(opacity);
	}
}

int RawDataPointmapRepresentationDialog::opacityPercent()
{
	return ui->transparencyWidget->opacity();
}

void RawDataPointmapRepresentationDialog::setHideBreakLines(bool hide)
{
    ui->hideBreaklinesCheckBox->setChecked(hide);
}

void RawDataPointmapRepresentationDialog::setPointSize(int size)
{
	ui->pointSizeSpinBox->setValue(size);
}

bool RawDataPointmapRepresentationDialog::hideBreakLines()
{
    return ui->hideBreaklinesCheckBox->isChecked();
}

int RawDataPointmapRepresentationDialog::pointSize()
{
	return ui->pointSizeSpinBox->value();
}
