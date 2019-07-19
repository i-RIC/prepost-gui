#include "ui_distancemeasurepropertydialog.h"

#include "distancemeasurepropertydialog.h"

#include <QVector2D>

DistanceMeasurePropertyDialog::DistanceMeasurePropertyDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::DistanceMeasurePropertyDialog}
{
	ui->setupUi(this);
	ui->labelFontSettingWidget->hideColor();

	connect(ui->startPointXEdit, SIGNAL(valueChanged(double)), this, SLOT(updateAutoLabel()));
	connect(ui->startPointYEdit, SIGNAL(valueChanged(double)), this, SLOT(updateAutoLabel()));
	connect(ui->endPointXEdit, SIGNAL(valueChanged(double)), this, SLOT(updateAutoLabel()));
	connect(ui->endPointYEdit, SIGNAL(valueChanged(double)), this, SLOT(updateAutoLabel()));
}

DistanceMeasurePropertyDialog::~DistanceMeasurePropertyDialog()
{
	delete ui;
}

QString DistanceMeasurePropertyDialog::name() const
{
	return ui->nameLineEdit->text();
}

void DistanceMeasurePropertyDialog::setName(const QString& name)
{
	ui->nameLineEdit->setText(name.trimmed());
}

DistanceMeasureSetting DistanceMeasurePropertyDialog::setting() const
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

	ret.labelFontSetting = ui->labelFontSettingWidget->setting();
	ret.customLabel = ui->customLabelLineEdit->text();

	ret.showMarkers = ui->showMarkersCheckBox->isChecked();
	ret.markerSize = ui->markerSizeSpinBox->value();

	ret.color = ui->colorWidget->color();
	ret.labelFontSetting.fontColor = ret.color;

	return ret;
}

void DistanceMeasurePropertyDialog::setSetting(const DistanceMeasureSetting& setting)
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

	ui->labelFontSettingWidget->setSetting(setting.labelFontSetting);
	ui->customLabelLineEdit->setText(setting.customLabel);

	ui->showMarkersCheckBox->setChecked(setting.showMarkers);
	ui->markerSizeSpinBox->setValue(setting.markerSize);

	ui->colorWidget->setColor(setting.color);
}

void DistanceMeasurePropertyDialog::updateAutoLabel()
{
	ui->autoLabelLabel->setText(autoLabel());
}

QString DistanceMeasurePropertyDialog::autoLabel() const
{
	QVector2D v1(ui->startPointXEdit->value(), ui->startPointYEdit->value());
	QVector2D v2(ui->endPointXEdit->value(), ui->endPointYEdit->value());

	QVector2D v = v2 - v1;
	return QString::number(v.length());
}
