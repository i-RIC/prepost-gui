#include "post2dwindownodevectorarrowgroupunstructureddataitem_propertydialog_additionalwidgets.h"
#include "ui_post2dwindownodevectorarrowgroupunstructureddataitem_propertydialog_additionalwidgets.h"

Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::PropertyDialog::AdditionalWidgets::AdditionalWidgets(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Post2dWindowNodeVectorArrowGroupUnstructuredDataItem_PropertyDialog_AdditionalWidgets)
{
	ui->setupUi(this);
}

Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::PropertyDialog::AdditionalWidgets::~AdditionalWidgets()
{
	delete ui;
}

GeneralFilteringSettingEditWidget* Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::PropertyDialog::AdditionalWidgets::samplingWidget() const
{
	return ui->samplingWidget;
}

Region2dSettingEditWidget* Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::PropertyDialog::AdditionalWidgets::regionWidget() const
{
	return ui->regionWidget;
}
