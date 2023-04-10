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

	connect(ui->importButton, &QPushButton::clicked, [=](bool){widget->importSetting();});
	connect(ui->exportButton, &QPushButton::clicked, [=](bool){widget->exportSetting();});
}

ColorMapSettingEditWidgetWithImportExportButton::~ColorMapSettingEditWidgetWithImportExportButton()
{
	delete m_widget;
	delete ui;
}

QUndoCommand* ColorMapSettingEditWidgetWithImportExportButton::createModifyCommand(bool /*apply*/)
{
	return m_widget->createModifyCommand();
}
