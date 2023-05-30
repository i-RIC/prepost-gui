#include "post2dwindownodevectorarrowgroupunstructureddataitem_settingeditwidget_additionalwidgets.h"
#include "ui_post2dwindownodevectorarrowgroupunstructureddataitem_settingeditwidget_additionalwidgets.h"

Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::SettingEditWidget::AdditionalWidgets::AdditionalWidgets(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Post2dWindowNodeVectorArrowGroupUnstructuredDataItem_SettingEditWidget_AdditionalWidgets)
{
	ui->setupUi(this);
}

Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::SettingEditWidget::AdditionalWidgets::~AdditionalWidgets()
{
	delete ui;
}

GeneralFilteringSettingEditWidget* Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::SettingEditWidget::AdditionalWidgets::samplingWidget() const
{
	return ui->samplingWidget;
}

Region2dSettingEditWidget* Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::SettingEditWidget::AdditionalWidgets::regionWidget() const
{
	return ui->regionWidget;
}
