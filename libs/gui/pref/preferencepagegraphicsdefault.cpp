#include "ui_preferencepagegraphicsdefault.h"

#include "preferencepagegraphicsdefault.h"

#include <guicore/arrows/arrowssettingcontainer.h>
#include <guicore/project/backgroundimageinfo.h>

PreferencePageGraphicsDefault::PreferencePageGraphicsDefault(QWidget* parent) :
	PreferencePage(parent),
	ui(new Ui::PreferencePageGraphicsDefault)
{
	ui->setupUi(this);
	m_settings.beginGroup("graphics");

	ui->gridColorWidget->setColor(m_settings.value("gridcolor", QColor(Qt::black)).value<QColor>());

	ui->vectorColorWidget->setColor(m_settings.value("arrow_color", QColor(Qt::black)).value<QColor>());
	ui->vectorArrowSizeSpinBox->setValue(m_settings.value("arrow_size", ArrowsSettingContainer::DEFAULT_ARROWSIZE).toInt());
	ui->vectorArrowLineWidthSpinBox->setValue(m_settings.value("arrow_linewidth", ArrowsSettingContainer::DEFAULT_LINEWIDTH).toInt());

	ui->streamLineColorWidget->setColor(m_settings.value("streamline_color", QColor(Qt::black)).value<QColor>());
	ui->streamlineWidthSpinBox->setValue(m_settings.value("streamline_width", 1).toInt());

	ui->particleColorWidget->setColor(m_settings.value("particle_color", QColor(Qt::black)).value<QColor>());
	ui->particleSizeSpinBox->setValue(m_settings.value("particle_size", 3).toInt());

	ui->backgroundSizeSpinBox->setValue(m_settings.value("backgroundimagemaxsize", BackgroundImageInfo::MAXWIDTH).toInt());
}

PreferencePageGraphicsDefault::~PreferencePageGraphicsDefault()
{
	delete ui;
}

void PreferencePageGraphicsDefault::update()
{
	m_settings.setValue("gridcolor", ui->gridColorWidget->color());

	m_settings.setValue("arrow_color", ui->vectorColorWidget->color());
	m_settings.setValue("arrow_size", ui->vectorArrowSizeSpinBox->value());
	m_settings.setValue("arrow_linewidth", ui->vectorArrowLineWidthSpinBox->value());

	m_settings.setValue("streamline_color", ui->streamLineColorWidget->color());
	m_settings.setValue("streamline_width", ui->streamlineWidthSpinBox->value());

	m_settings.setValue("particle_color", ui->particleColorWidget->color());
	m_settings.setValue("particle_size", ui->particleSizeSpinBox->value());

	m_settings.setValue("backgroundimagemaxsize", ui->backgroundSizeSpinBox->value());
}
