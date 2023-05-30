#include "post2dwindownodevectorarrowgroupstructureddataitem_settingeditwidget_additionalwidgets.h"
#include "ui_post2dwindownodevectorarrowgroupstructureddataitem_settingeditwidget_additionalwidgets.h"

Post2dWindowNodeVectorArrowGroupStructuredDataItem::SettingEditWidget::AdditionalWidgets::AdditionalWidgets(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Post2dWindowNodeVectorArrowGroupStructuredDataItem_SettingEditWidget_AdditionalWidgets)
{
	ui->setupUi(this);

	connect(ui->regionWidget, &Region2dSettingEditWidget::modeChanged, this, &AdditionalWidgets::handleRegionModeChange);
}

Post2dWindowNodeVectorArrowGroupStructuredDataItem::SettingEditWidget::AdditionalWidgets::~AdditionalWidgets()
{
	delete ui;
}

Structured2dFilteringSettingEditWidget* Post2dWindowNodeVectorArrowGroupStructuredDataItem::SettingEditWidget::AdditionalWidgets::samplingWidget() const
{
	return ui->samplingWidget;
}

Region2dSettingEditWidget* Post2dWindowNodeVectorArrowGroupStructuredDataItem::SettingEditWidget::AdditionalWidgets::regionWidget() const
{
	return ui->regionWidget;
}

void Post2dWindowNodeVectorArrowGroupStructuredDataItem::SettingEditWidget::AdditionalWidgets::handleRegionModeChange()
{
	auto rSetting = ui->regionWidget->setting();
	if (rSetting.mode.value() == Region2dSettingContainer::Mode::Active) {
		ui->samplingWidget->setModeToAll();
	}
}
