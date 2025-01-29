#include "colormapsettingeditwidgeti.h"
#include "colormapsettingeditwidgetwithimportexportbutton.h"
#include "ui_colormapsettingeditwidgetwithimportexportbutton.h"

ColorMapSettingEditWidgetWithImportExportButton::ColorMapSettingEditWidgetWithImportExportButton(ColorMapSettingEditWidgetI* widget, QWidget *parent) :
	ModifyCommandWidget(parent),
	m_widget {widget},
	ui(new Ui::ColorMapSettingEditWidgetWithImportExportButton)
{
	ui->setupUi(this);
	ui->widget->setWidget(widget);
	ui->setAsDefaultButton->hide();
	ui->setAsDefaultLabel->hide();

	connect(ui->importButton, &QPushButton::clicked, widget, &ColorMapSettingEditWidgetI::importSetting);
	connect(ui->exportButton, &QPushButton::clicked, widget, &ColorMapSettingEditWidgetI::exportSetting);
	connect(ui->setAsDefaultButton, &QPushButton::clicked, this, &ColorMapSettingEditWidgetWithImportExportButton::setAsDefaultClicked);
}

ColorMapSettingEditWidgetWithImportExportButton::~ColorMapSettingEditWidgetWithImportExportButton()
{
	delete m_widget;
	delete ui;
}

ColorMapSettingEditWidgetI* ColorMapSettingEditWidgetWithImportExportButton::widget() const
{
	return m_widget;
}

void ColorMapSettingEditWidgetWithImportExportButton::showSetAsDefaultButton()
{
	ui->setAsDefaultButton->show();
	ui->setAsDefaultLabel->show();
}

QUndoCommand* ColorMapSettingEditWidgetWithImportExportButton::createModifyCommand(bool /*apply*/)
{
	return m_widget->createModifyCommand();
}
