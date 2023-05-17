#include "post2dwindownodevectorparticlegroupstructureddataitem_settingeditwidget_startpositioneditwidget.h"
#include "ui_post2dwindownodevectorparticlegroupstructureddataitem_settingeditwidget_startpositioneditwidget.h"

Post2dWindowNodeVectorParticleGroupStructuredDataItem::SettingEditWidget::StartPositionEditWidget::StartPositionEditWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Post2dWindowNodeVectorParticleGroupStructuredDataItem_SettingEditWidget_StartPositionEditWidget)
{
	ui->setupUi(this);
}

Post2dWindowNodeVectorParticleGroupStructuredDataItem::SettingEditWidget::StartPositionEditWidget::~StartPositionEditWidget()
{
	delete ui;
}

void Post2dWindowNodeVectorParticleGroupStructuredDataItem::SettingEditWidget::StartPositionEditWidget::setDimensions(int idim, int jdim)
{
	ui->rangeWidget->setDimensions(idim, jdim);
}

Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting::StartPosition Post2dWindowNodeVectorParticleGroupStructuredDataItem::SettingEditWidget::StartPositionEditWidget::setting() const
{
	Setting::StartPosition ret;
	ret.range = ui->rangeWidget->setting();
	ret.skipOrSubdivide = ui->skipWidget->setting();
	ret.color = ui->colorWidget->color();
	ret.pointSize = ui->pointSizeSpinBox->value();

	return ret;
}

void Post2dWindowNodeVectorParticleGroupStructuredDataItem::SettingEditWidget::StartPositionEditWidget::setSetting(const Setting::StartPosition& pos)
{
	ui->rangeWidget->setSetting(pos.range);
	ui->skipWidget->setSetting(pos.skipOrSubdivide);
	ui->colorWidget->setColor(pos.color);
	ui->pointSizeSpinBox->setValue(pos.pointSize);
}
