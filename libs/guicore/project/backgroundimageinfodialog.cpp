#include "ui_backgroundimageinfodialog.h"

#include "backgroundimageinfo.h"
#include "backgroundimageinfodialog.h"
#include "private/backgroundimageinfo_impl.h"

#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>

BackgroundImageInfoDialog::BackgroundImageInfoDialog(BackgroundImageInfo* info) :
	QDialog(nullptr),
	ui(new Ui::BackgroundImageInfoDialog),
	m_info {info}
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

void BackgroundImageInfoDialog::setImageWidth(int width)
{
	m_imageWidth = width;
}

BackgroundImageInfo::Setting BackgroundImageInfoDialog::BackgroundImageInfoDialog::setting() const
{
	BackgroundImageInfo::Setting s = m_originalSetting;

	s.positionX = ui->blxSpinBox->value();
	s.positionY = ui->blySpinBox->value();
	s.scale = ui->scaleSpinBox->value();
	s.angle = ui->angleSpinBox->value();

	return s;
}

void BackgroundImageInfoDialog::setSetting(const BackgroundImageInfo::Setting& setting)
{
	m_originalSetting = setting;
	applySettingToWidgets(setting);
}

void BackgroundImageInfoDialog::update()
{
	if (ui->bottomRightRadioButton->isChecked()) {
		updateScaleAndAngle();
	} else {
		updateRightBottom();
	}
}

void BackgroundImageInfoDialog::applySettingToWidgets(const BackgroundImageInfo::Setting& setting)
{
	ui->blxSpinBox->setValue(setting.positionX);
	ui->blySpinBox->setValue(setting.positionY);
	ui->scaleSpinBox->setValue(setting.scale);
	ui->angleSpinBox->setValue(setting.angle);

	updateRightBottom();
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

void BackgroundImageInfoDialog::reset()
{
	m_info->fitImageToData();

	ui->blxSpinBox->blockSignals(true);
	ui->blySpinBox->blockSignals(true);
	ui->brxSpinBox->blockSignals(true);
	ui->brySpinBox->blockSignals(true);
	ui->scaleSpinBox->blockSignals(true);
	ui->angleSpinBox->blockSignals(true);

	applySettingToWidgets(m_info->impl->m_setting);
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
	m_info->impl->m_setting = setting();
	m_info->informChange();
}

void BackgroundImageInfoDialog::reject()
{
	m_info->impl->m_setting = m_originalSetting;
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
