#include "post3dwindownodevectorstreamlinegroupstructureddataitem_settingeditwidget_startpositioneditwidget.h"
#include "ui_post3dwindownodevectorstreamlinegroupstructureddataitem_settingeditwidget_startpositioneditwidget.h"

Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::StartPositionEditWidget::StartPositionEditWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Post3dWindowNodeVectorStreamlineGroupStructuredDataItem_SettingEditWidget_StartPositionEditWidget)
{
	ui->setupUi(this);
}

Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::StartPositionEditWidget::~StartPositionEditWidget()
{
	delete ui;
}

void Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::StartPositionEditWidget::setDimensions(int* dims)
{
	ui->faceWidget->setDimensions(dims);
}

Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting::StartPosition Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::StartPositionEditWidget::setting() const
{
	Setting::StartPosition ret;
	ret.face = ui->faceWidget->setting();
	ret.skipOrSubdivide = ui->skipWidget->setting();
	ret.color = ui->colorWidget->color();
	ret.width = ui->widthSpinBox->value();

	return ret;
}

void Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::StartPositionEditWidget::setSetting(const Setting::StartPosition& pos)
{
	ui->faceWidget->setSetting(pos.face);
	ui->skipWidget->setSetting(pos.skipOrSubdivide);
	ui->colorWidget->setColor(pos.color);
	ui->widthSpinBox->setValue(pos.width);
}
