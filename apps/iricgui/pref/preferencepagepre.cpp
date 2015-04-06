#include "preferencepagepre.h"
#include "ui_preferencepagepre.h"

#include <QColor>

PreferencePagePre::PreferencePagePre(QWidget *parent) :
    PreferencePage(parent),
    ui(new Ui::PreferencePagePre)
{
    ui->setupUi(this);
	m_settings.beginGroup("preprocessor");

	QColor gridColor = m_settings.value("gridcolor", QColor(Qt::black)).value<QColor>();
	ui->gridColorWidget->setColor(gridColor);

	int cm = m_settings.value("colormap", static_cast<int>(ColorMapSettingWidget::Rainbow)).value<int>();
	ColorMapSettingWidget::ColorMap cmVal = static_cast<ColorMapSettingWidget::ColorMap>(cm);
	ui->colorMapWidget->setColorMap(cmVal);
}

PreferencePagePre::~PreferencePagePre()
{
    delete ui;
}

void PreferencePagePre::update()
{
	m_settings.setValue("gridcolor", ui->gridColorWidget->color());
	ColorMapSettingWidget::ColorMap cm = ui->colorMapWidget->colorMap();
	m_settings.setValue("colormap", static_cast<int>(cm));
}
