#include "post2dwindownodevectorstreamlinegroupstructureddataitem_settingeditwidget_startpositioneditwidget.h"
#include "ui_post2dwindownodevectorstreamlinegroupstructureddataitem_settingeditwidget_startpositioneditwidget.h"

Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::StartPositionEditWidget::StartPositionEditWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Post2dWindowNodeVectorStreamlineGroupStructuredDataItem_SettingEditWidget_StartPositionEditWidget)
{
	ui->setupUi(this);
}

Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::StartPositionEditWidget::~StartPositionEditWidget()
{
	delete ui;
}

void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::StartPositionEditWidget::setDimensions(int idim, int jdim)
{
	ui->rangeWidget->setDimensions(idim, jdim);
}

Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting::StartPosition Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::StartPositionEditWidget::setting() const
{
	Setting::StartPosition ret;
	ret.range = ui->rangeWidget->setting();
	ret.skipOrSubdivide = ui->skipWidget->setting();
	ret.color = ui->colorWidget->color();
	ret.width = ui->widthSpinBox->value();

	return ret;
}

void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::StartPositionEditWidget::setSetting(const Setting::StartPosition& pos)
{
	ui->rangeWidget->setSetting(pos.range);
	ui->skipWidget->setSetting(pos.skipOrSubdivide);
	ui->colorWidget->setColor(pos.color);
	ui->widthSpinBox->setValue(pos.width);
}
