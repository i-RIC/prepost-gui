#include "ui_preferencepagegraphicsdefault.h"

#include "preferencepagegraphicsdefault.h"

#include <guicore/project/backgroundimageinfo.h>

PreferencePageGraphicsDefault::PreferencePageGraphicsDefault(QWidget* parent) :
	PreferencePage(parent),
	ui(new Ui::PreferencePageGraphicsDefault)
{
	ui->setupUi(this);
	m_settings.beginGroup("graphics");

	ui->gridColorWidget->setColor(m_settings.value("gridcolor", QColor(Qt::black)).value<QColor>());

	int cm = m_settings.value("colormap", static_cast<int>(ColorMapSettingWidget::Rainbow)).value<int>();
	ColorMapSettingWidget::ColorMap cmVal = static_cast<ColorMapSettingWidget::ColorMap>(cm);
	ui->colorMapWidget->setColorMap(cmVal);
	ui->colorMapWidget->disableCustom();

	ui->vectorFactor->setValue(m_settings.value("vectorfactor", 1.0).value<double>());
	ui->vectorColorWidget->setColor(m_settings.value("vectorcolor", QColor(Qt::black)).value<QColor>());
	ui->vectorArrowSizeSpinBox->setValue(m_settings.value("vectorarrowsize", 3).value<int>());

	ui->streamLineColorWidget->setColor(m_settings.value("streamlinecolor", QColor(Qt::yellow)).value<QColor>());

	ui->particleColorWidget->setColor(m_settings.value("particlecolor", QColor(Qt::black)).value<QColor>());
	ui->particleSizeSpinBox->setValue(m_settings.value("particlesize", 3).value<int>());

	ui->backgroundSizeSpinBox->setValue(m_settings.value("backgroundimagemaxsize", BackgroundImageInfo::MAXWIDTH).value<int>());
}

PreferencePageGraphicsDefault::~PreferencePageGraphicsDefault()
{
	delete ui;
}

void PreferencePageGraphicsDefault::update()
{
	m_settings.setValue("gridcolor", ui->gridColorWidget->color());

	ColorMapSettingWidget::ColorMap cm = ui->colorMapWidget->colorMap();
	m_settings.setValue("colormap", static_cast<int>(cm));

	m_settings.setValue("vectorfactor", ui->vectorFactor->value());
	m_settings.setValue("vectorcolor", ui->vectorColorWidget->color());
	m_settings.setValue("vectorarrowsize", ui->vectorArrowSizeSpinBox->value());

	m_settings.setValue("streamlinecolor", ui->streamLineColorWidget->color());

	m_settings.setValue("particlecolor", ui->particleColorWidget->color());
	m_settings.setValue("particlesize", ui->particleSizeSpinBox->value());

	m_settings.setValue("backgroundimagemaxsize", ui->backgroundSizeSpinBox->value());
}
