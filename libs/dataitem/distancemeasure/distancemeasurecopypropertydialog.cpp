#include "ui_distancemeasurecopypropertydialog.h"

#include "distancemeasurecopypropertydialog.h"

#include <QVector2D>

DistanceMeasureCopyPropertyDialog::DistanceMeasureCopyPropertyDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::DistanceMeasureCopyPropertyDialog}
{
	ui->setupUi(this);
	connect(ui->startPointXEdit, SIGNAL(valueChanged(double)), this, SLOT(updateAutoLabel()));
	connect(ui->startPointYEdit, SIGNAL(valueChanged(double)), this, SLOT(updateAutoLabel()));
	connect(ui->endPointXEdit, SIGNAL(valueChanged(double)), this, SLOT(updateAutoLabel()));
	connect(ui->endPointYEdit, SIGNAL(valueChanged(double)), this, SLOT(updateAutoLabel()));
}

DistanceMeasureCopyPropertyDialog::~DistanceMeasureCopyPropertyDialog()
{
	delete ui;
}

QString DistanceMeasureCopyPropertyDialog::name() const
{
	return ui->nameLineEdit->text();
}

void DistanceMeasureCopyPropertyDialog::setName(const QString& name)
{
	ui->nameLineEdit->setText(name.trimmed());
}

DistanceMeasureSetting DistanceMeasureCopyPropertyDialog::setting() const
{
	DistanceMeasureSetting ret = m_setting;

	ret.point1 = QPointF(ui->startPointXEdit->value(), ui->startPointYEdit->value());
	ret.point2 = QPointF(ui->endPointXEdit->value(), ui->endPointYEdit->value());

	ret.showLabel = ui->showLabelCheckBox->isChecked();

	if (ui->labelAutoRadioButton->isChecked()) {
		ret.labelMode = DistanceMeasureSetting::Auto;
	} else if (ui->labelCustomRadioButton->isChecked()) {
		ret.labelMode = DistanceMeasureSetting::Custom;
	}

	if (ui->lpCenterTop->isChecked()) {
		ret.labelPosition = vtkLabel2DActor::lpTopCenter;
	} else if (ui->lpCenterBottom->isChecked()) {
		ret.labelPosition = vtkLabel2DActor::lpBottomCenter;
	} else if (ui->lpMiddleLeft->isChecked()) {
		ret.labelPosition = vtkLabel2DActor::lpMiddleLeft;
	} else if (ui->lpMiddleRight->isChecked()) {
		ret.labelPosition = vtkLabel2DActor::lpMiddleRight;
	}

	ret.labelFontSize = ui->fontSizeSpinBox->value();
	ret.customLabel = ui->customLabelLineEdit->text();

	ret.showMarkers = ui->showMarkersCheckBox->isChecked();
	ret.markerSize = ui->markerSizeSpinBox->value();

	ret.color = ui->colorWidget->color();

	return ret;
}

void DistanceMeasureCopyPropertyDialog::setSetting(const DistanceMeasureSetting& setting)
{
	m_setting = setting;

	ui->startPointXEdit->setValue(setting.point1.value().x());
	ui->startPointYEdit->setValue(setting.point1.value().y());
	ui->endPointXEdit->setValue(setting.point2.value().x());
	ui->endPointYEdit->setValue(setting.point2.value().y());

	ui->showLabelCheckBox->setChecked(setting.showLabel);

	if (setting.labelMode == DistanceMeasureSetting::Auto) {
		ui->labelAutoRadioButton->setChecked(true);
	} else {
		ui->labelCustomRadioButton->setChecked(true);
	}

	switch (setting.labelPosition.value()) {
	case vtkLabel2DActor::lpTopCenter:
		ui->lpCenterTop->setChecked(true);
		break;
	case vtkLabel2DActor::lpBottomCenter:
		ui->lpCenterBottom->setChecked(true);
		break;
	case vtkLabel2DActor::lpMiddleLeft:
		ui->lpMiddleLeft->setChecked(true);
		break;
	case vtkLabel2DActor::lpMiddleRight:
		ui->lpMiddleRight->setChecked(true);
		break;
	default:
		ui->lpCenterTop->setChecked(true);
	}

	ui->fontSizeSpinBox->setValue(setting.labelFontSize);
	ui->customLabelLineEdit->setText(setting.customLabel);

	ui->showMarkersCheckBox->setChecked(setting.showMarkers);
	ui->markerSizeSpinBox->setValue(setting.markerSize);

	ui->colorWidget->setColor(setting.color);
}

void DistanceMeasureCopyPropertyDialog::updateAutoLabel()
{
	ui->autoLabelLabel->setText(autoLabel());
}

QString DistanceMeasureCopyPropertyDialog::autoLabel() const
{
	QVector2D v1(ui->startPointXEdit->value(), ui->startPointYEdit->value());
	QVector2D v2(ui->endPointXEdit->value(), ui->endPointYEdit->value());

	QVector2D v = v2 - v1;
	return QString::number(v.length());
}
