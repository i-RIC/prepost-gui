#include "post2dwindownodevectorarrowgroupstructureddataitem_propertydialog_additionalwidgets.h"
#include "ui_post2dwindownodevectorarrowgroupstructureddataitem_propertydialog_additionalwidgets.h"

Post2dWindowNodeVectorArrowGroupStructuredDataItem::PropertyDialog::AdditionalWidgets::AdditionalWidgets(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Post2dWindowNodeVectorArrowGroupStructuredDataItem_PropertyDialog_AdditionalWidgets)
{
	ui->setupUi(this);

	connect(ui->regionWidget, &Region2dSettingEditWidget::modeChanged, this, &AdditionalWidgets::handleRegionModeChange);
}

Post2dWindowNodeVectorArrowGroupStructuredDataItem::PropertyDialog::AdditionalWidgets::~AdditionalWidgets()
{
	delete ui;
}

Structured2dFilteringSettingEditWidget* Post2dWindowNodeVectorArrowGroupStructuredDataItem::PropertyDialog::AdditionalWidgets::samplingWidget() const
{
	return ui->samplingWidget;
}

Region2dSettingEditWidget* Post2dWindowNodeVectorArrowGroupStructuredDataItem::PropertyDialog::AdditionalWidgets::regionWidget() const
{
	return ui->regionWidget;
}

void Post2dWindowNodeVectorArrowGroupStructuredDataItem::PropertyDialog::AdditionalWidgets::handleRegionModeChange()
{
	auto rSetting = ui->regionWidget->setting();
	if (rSetting.mode.value() == Region2dSettingContainer::Mode::Active) {
		ui->samplingWidget->setModeToAll();
	}
}
