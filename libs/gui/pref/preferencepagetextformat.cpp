#include "preferencepagetextformat.h"
#include "ui_preferencepagetextformat.h"

#include <QSettings>

PreferencePageTextFormat::PreferencePageTextFormat(QWidget *parent) :
	PreferencePage(parent),
	ui(new Ui::PreferencePageTextFormat)
{
	ui->setupUi(this);

	QSettings settings;

	ui->statusBarDecimalSpinBox->setValue(settings.value("textformat/statusbarcoordsdecimal", 6).toInt());

	ui->attributeBrowserCoordsSpinBox->setValue(settings.value("textformat/attbrowsercoordsdecimal", 6).toInt());
	ui->attributeBrowserValueSpinBox->setValue(settings.value("textformat/attbrowservaluedecimal", 6).toInt());
}

PreferencePageTextFormat::~PreferencePageTextFormat()
{
	delete ui;
}

void PreferencePageTextFormat::update()
{
	QSettings settings;

	settings.setValue("textformat/statusbarcoordsdecimal", ui->statusBarDecimalSpinBox->value());

	settings.setValue("textformat/attbrowsercoordsdecimal", ui->attributeBrowserCoordsSpinBox->value());
	settings.setValue("textformat/attbrowservaluedecimal", ui->attributeBrowserValueSpinBox->value());
}
