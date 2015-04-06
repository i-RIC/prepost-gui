#include "preferencepagepost2d.h"
#include "ui_preferencepagepost2d.h"

PreferencePagePost2d::PreferencePagePost2d(QWidget *parent) :
    PreferencePage(parent),
    ui(new Ui::PreferencePagePost2d)
{
    ui->setupUi(this);
	m_settings.beginGroup("post2d");
	ui->gridColorWidget->setColor(m_settings.value("gridcolor", QColor(Qt::black)).value<QColor>());

	ui->vectorFactor->setValue(m_settings.value("vectorfactor", 1.0).value<double>());
	ui->vectorColorWidget->setColor(m_settings.value("vectorcolor", QColor(Qt::black)).value<QColor>());
	ui->vectorArrowSizeSpinBox->setValue(m_settings.value("vectorarrowsize", 3).value<int>());

	ui->streamLineColorWidget->setColor(m_settings.value("streamlinecolor", QColor(Qt::yellow)).value<QColor>());

	ui->particleColorWidget->setColor(m_settings.value("particlecolor", QColor(Qt::black)).value<QColor>());
	ui->particleSizeSpinBox->setValue(m_settings.value("particlesize", 3).value<int>());
}

PreferencePagePost2d::~PreferencePagePost2d()
{
    delete ui;
}

void PreferencePagePost2d::update()
{
	m_settings.setValue("gridcolor", ui->gridColorWidget->color());

	m_settings.setValue("vectorfactor", ui->vectorFactor->value());
	m_settings.setValue("vectorcolor", ui->vectorColorWidget->color());
	m_settings.setValue("vectorarrowsize", ui->vectorArrowSizeSpinBox->value());

	m_settings.setValue("streamlinecolor", ui->streamLineColorWidget->color());

	m_settings.setValue("particlecolor", ui->particleColorWidget->color());
	m_settings.setValue("particlesize", ui->particleSizeSpinBox->value());
}
