#include "colormaplegendsettingcontaineri.h"
#include "colormapsettingcontaineri.h"
#include "delegatedcolormapsettingcontainer.h"
#include "delegatedcolormapsettingeditwidget.h"
#include "ui_delegatedcolormapsettingeditwidget.h"

#include <guicore/scalarstocolors/colormapsettingeditwidgeti.h>
#include <misc/valuemodifycommandt.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>

DelegatedColorMapSettingEditWidget::DelegatedColorMapSettingEditWidget(QWidget *parent) :
	ModifyCommandWidget(parent),
	m_widget {nullptr},
	m_setting {nullptr},
	ui(new Ui::DelegatedColorMapSettingEditWidget)
{
	ui->setupUi(this);
	connect(ui->useOriginalCheckBox, &QCheckBox::toggled, this, &DelegatedColorMapSettingEditWidget::setUseOriginalSetting);
}

DelegatedColorMapSettingEditWidget::~DelegatedColorMapSettingEditWidget()
{
	delete ui;
}

void DelegatedColorMapSettingEditWidget::setEditWidget(ColorMapSettingEditWidgetI* widget)
{
	m_widget = widget;
	ui->colorMapWidget->setWidget(widget);

	connect(ui->importButton, &QPushButton::clicked, [=](bool){m_widget->importSetting();});
	connect(ui->exportButton, &QPushButton::clicked, [=](bool){m_widget->exportSetting();});
}

void DelegatedColorMapSettingEditWidget::setSetting(DelegatedColorMapSettingContainer* setting)
{
	m_setting = setting;

	ui->useOriginalCheckBox->setChecked(setting->usePreSetting);
	setUseOriginalSetting(setting->usePreSetting);
}

QUndoCommand* DelegatedColorMapSettingEditWidget::createModifyCommand(bool apply)
{
	BoolContainer useOriginal("useOriginal");
	useOriginal = ui->useOriginalCheckBox->isChecked();

	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("DelegatedColorMapSettingEditWidget::Modify"), apply);
	command->addCommand(new ValueModifyCommmand<BoolContainer>(iRIC::generateCommandId("UseOriginal"), true, useOriginal, &m_setting->usePreSetting));
	command->addCommand(m_widget->createModifyCommand());
	return command;
}

void DelegatedColorMapSettingEditWidget::setUseOriginalSetting(bool use)
{
	m_widget->setDisableOtherThanImageSetting(use);
	if (use) {
		m_widget->setLegendSetting(m_setting->preSetting);
	} else {
		m_setting->customSetting->copy(*m_setting->preSetting->setting());
		m_setting->customSetting->legendSetting()->copy(*m_setting->preSetting);

		m_widget->setSetting(m_setting->customSetting);
	}
}
