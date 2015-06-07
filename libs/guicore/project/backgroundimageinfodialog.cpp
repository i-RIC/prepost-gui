#include "ui_backgroundimageinfodialog.h"

#include "backgroundimageinfo.h"
#include "backgroundimageinfodialog.h"

#include <cmath>

#define M_PI 3.1415926535

BackgroundImageInfoDialog::BackgroundImageInfoDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::BackgroundImageInfoDialog)
{
	ui->setupUi(this);
	connect(ui->blxSpinBox, SIGNAL(valueChanged(double)), this, SLOT(update()));
	connect(ui->blySpinBox, SIGNAL(valueChanged(double)), this, SLOT(update()));
	connect(ui->brxSpinBox, SIGNAL(valueChanged(double)), this, SLOT(update()));
	connect(ui->brySpinBox, SIGNAL(valueChanged(double)), this, SLOT(update()));
	connect(ui->scaleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(update()));
	connect(ui->angleSpinBox, SIGNAL(valueChanged(double)), this, SLOT(update()));
	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonBoxClick(QAbstractButton*)));
}

BackgroundImageInfoDialog::~BackgroundImageInfoDialog()
{
	delete ui;
}

void BackgroundImageInfoDialog::setProperties(double lbX, double lbY, double scale, double angle)
{
	m_leftbottomX = lbX;
	m_leftbottomY = lbY;
	m_scale = scale;
	m_angle = angle;

	m_origLeftbottomX = lbX;
	m_origLeftbottomY = lbY;
	m_origScale = scale;
	m_origAngle = angle;

	ui->blxSpinBox->setValue(lbX);
	ui->blySpinBox->setValue(lbY);
	ui->scaleSpinBox->setValue(m_scale);
	ui->angleSpinBox->setValue(m_angle);
	updateRightBottom();
}

void BackgroundImageInfoDialog::update()
{
	if (ui->bottomRightRadioButton->isChecked()) {
		updateScaleAndAngle();
	} else {
		updateRightBottom();
	}
}

void BackgroundImageInfoDialog::updateScaleAndAngle()
{
	double xdist = ui->brxSpinBox->value() - ui->blxSpinBox->value();
	double ydist = ui->brySpinBox->value() - ui->blySpinBox->value();
	double distance = std::sqrt(xdist * xdist + ydist * ydist);

	double scale = distance / m_imageWidth;
	ui->scaleSpinBox->setValue(scale);
	double rate = ydist / xdist;
	double angle = 0;
	if (xdist == 0 || std::abs(rate) > 1E6) {
		if (ydist > 0) {
			angle = 90;
		} else {
			angle = -90;
		}
	} else {
		angle = std::atan(rate) / M_PI * 180;
		if (xdist < 0) {
			if (ydist > 0) {
				angle += 180;
			} else {
				angle -= 180;
			}
		}
	}
	ui->angleSpinBox->setValue(angle);
}

void BackgroundImageInfoDialog::updateRightBottom()
{
	double rbx = ui->blxSpinBox->value() + std::cos(ui->angleSpinBox->value() / 180. * M_PI) * (ui->scaleSpinBox->value() * m_imageWidth);
	double rby = ui->blySpinBox->value() + std::sin(ui->angleSpinBox->value() / 180. * M_PI) * (ui->scaleSpinBox->value() * m_imageWidth);

	ui->brxSpinBox->setValue(rbx);
	ui->brySpinBox->setValue(rby);
}

void BackgroundImageInfoDialog::accept()
{
	m_leftbottomX = ui->blxSpinBox->value();
	m_leftbottomY = ui->blySpinBox->value();
	m_scale = ui->scaleSpinBox->value();
	m_angle = ui->angleSpinBox->value();
	QDialog::accept();
}

void BackgroundImageInfoDialog::reset()
{
	m_info->fitImageToData();
	ui->blxSpinBox->blockSignals(true);
	ui->blySpinBox->blockSignals(true);
	ui->brxSpinBox->blockSignals(true);
	ui->brySpinBox->blockSignals(true);
	ui->scaleSpinBox->blockSignals(true);
	ui->angleSpinBox->blockSignals(true);

	ui->blxSpinBox->setValue(m_info->translateX());
	ui->blySpinBox->setValue(m_info->translateY());
	ui->scaleSpinBox->setValue(m_info->scale());
	ui->angleSpinBox->setValue(m_info->angle());
	updateRightBottom();

	ui->blxSpinBox->blockSignals(false);
	ui->blySpinBox->blockSignals(false);
	ui->brxSpinBox->blockSignals(false);
	ui->brySpinBox->blockSignals(false);
	ui->scaleSpinBox->blockSignals(false);
	ui->angleSpinBox->blockSignals(false);

	m_info->informChange();
}

void BackgroundImageInfoDialog::apply()
{
	m_info->m_translateX = ui->blxSpinBox->value();
	m_info->m_translateY = ui->blySpinBox->value();
	m_info->m_scale = ui->scaleSpinBox->value();
	m_info->m_angle = ui->angleSpinBox->value();
	m_info->informChange();
}

void BackgroundImageInfoDialog::reject()
{
	m_info->m_translateX = m_origLeftbottomX;
	m_info->m_translateY = m_origLeftbottomY;
	m_info->m_scale = m_origScale;
	m_info->m_angle = m_origAngle;
	m_info->informChange();
	QDialog::reject();
}

void BackgroundImageInfoDialog::handleButtonBoxClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	} else if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ResetRole) {
		reset();
	}
}
