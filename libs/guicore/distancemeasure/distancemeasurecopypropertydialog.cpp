#include "ui_distancemeasurecopypropertydialog.h"

#include "distancemeasurecopypropertydialog.h"

#include <QVector2D>

DistanceMeasureCopyPropertyDialog::DistanceMeasureCopyPropertyDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::DistanceMeasureCopyPropertyDialog)
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

void DistanceMeasureCopyPropertyDialog::setShowLabel(bool show)
{
	ui->showLabelCheckBox->setChecked(show);
}

void DistanceMeasureCopyPropertyDialog::setName(const QString& name)
{
	ui->nameLineEdit->setText(name.trimmed());
}

void DistanceMeasureCopyPropertyDialog::setPoints(const QVector2D& v1, const QVector2D& v2)
{
	ui->startPointXEdit->setValue(v1.x());
	ui->startPointYEdit->setValue(v1.y());
	ui->endPointXEdit->setValue(v2.x());
	ui->endPointYEdit->setValue(v2.y());
}

void DistanceMeasureCopyPropertyDialog::setLabelPosition(LabelPosition pos)
{
	switch (pos) {
	case lpTopCenter:
		ui->lpCenterTop->setChecked(true);
		break;
	case lpBottomCenter:
		ui->lpCenterBottom->setChecked(true);
		break;
	case lpMiddleLeft:
		ui->lpMiddleLeft->setChecked(true);
		break;
	case lpMiddleRight:
		ui->lpMiddleRight->setChecked(true);
		break;
	default:
		ui->lpCenterTop->setChecked(true);
	}
}

void DistanceMeasureCopyPropertyDialog::setLabelMode(LabelMode lm)
{
	switch (lm) {
	case lmAuto:
		ui->labelAutoRadioButton->setChecked(true);
		break;
	case lmCustom:
		ui->labelCustomRadioButton->setChecked(true);
		break;
	default:
		ui->labelAutoRadioButton->setChecked(true);
	}
}

void DistanceMeasureCopyPropertyDialog::setCustomlabel(const QString& label)
{
	ui->customLabelLineEdit->setText(label);
}

void DistanceMeasureCopyPropertyDialog::setLabelFontSize(int fontSize)
{
	ui->fontSizeSpinBox->setValue(fontSize);
}

void DistanceMeasureCopyPropertyDialog::setShowMarkers(bool show)
{
	ui->showMarkersCheckBox->setChecked(show);
}

void DistanceMeasureCopyPropertyDialog::setMarkerSize(int size)
{
	ui->markerSizeSpinBox->setValue(size);
}

void DistanceMeasureCopyPropertyDialog::setColor(QColor color)
{
	ui->colorWidget->setColor(color);
}

QString DistanceMeasureCopyPropertyDialog::name() const
{
	return ui->nameLineEdit->text();
}

QVector2D DistanceMeasureCopyPropertyDialog::point1() const
{
	QVector2D ret(ui->startPointXEdit->value(), ui->startPointYEdit->value());
	return ret;
}

QVector2D DistanceMeasureCopyPropertyDialog::point2() const
{
	QVector2D ret(ui->endPointXEdit->value(), ui->endPointYEdit->value());
	return ret;
}

bool DistanceMeasureCopyPropertyDialog::showLabel() const
{
	return ui->showLabelCheckBox->isChecked();
}

DistanceMeasureCopyPropertyDialog::LabelPosition DistanceMeasureCopyPropertyDialog::labelPosition() const
{
	if (ui->lpCenterTop->isChecked()) {
		return lpTopCenter;
	} else if (ui->lpCenterBottom->isChecked()) {
		return lpBottomCenter;
	} else if (ui->lpMiddleLeft->isChecked()) {
		return lpMiddleLeft;
	} else if (ui->lpMiddleRight->isChecked()) {
		return lpMiddleRight;
	}
	return lpTopCenter;
}

DistanceMeasureCopyPropertyDialog::LabelMode DistanceMeasureCopyPropertyDialog::labelMode() const
{
	if (ui->labelAutoRadioButton->isChecked()) {
		return lmAuto;
	} else if (ui->labelCustomRadioButton->isChecked()) {
		return lmCustom;
	}
	return lmAuto;
}

QString DistanceMeasureCopyPropertyDialog::autoLabel() const
{
	QVector2D v = point2() - point1();
	return QString::number(v.length());
}

QString DistanceMeasureCopyPropertyDialog::customLabel() const
{
	return ui->customLabelLineEdit->text();
}

int DistanceMeasureCopyPropertyDialog::fontSize() const
{
	return ui->fontSizeSpinBox->value();
}

bool DistanceMeasureCopyPropertyDialog::showMarkers() const
{
	return ui->showMarkersCheckBox->isChecked();
}

int DistanceMeasureCopyPropertyDialog::markerSize() const
{
	return ui->markerSizeSpinBox->value();
}

QColor DistanceMeasureCopyPropertyDialog::color() const
{
	return ui->colorWidget->color();
}

void DistanceMeasureCopyPropertyDialog::updateAutoLabel()
{
	ui->autoLabelLabel->setText(autoLabel());
}
