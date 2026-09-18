#include "ui_preferencepagetelemetry.h"

#include "preferencepagetelemetry.h"

#include <misc/telemetrywidget.h>

PreferencePageTelemetry::PreferencePageTelemetry(iRICAuthClient* client, QWidget* parent) :
	PreferencePage(parent),
	m_widget {new TelemetryWidget(client, this)},
	ui(new Ui::PreferencePageTelemetry)
{
	ui->setupUi(this);
	ui->contentLayout->addWidget(m_widget);
}

PreferencePageTelemetry::~PreferencePageTelemetry()
{
	delete ui;
}

void PreferencePageTelemetry::update()
{
	m_widget->applyToSettings();
}
