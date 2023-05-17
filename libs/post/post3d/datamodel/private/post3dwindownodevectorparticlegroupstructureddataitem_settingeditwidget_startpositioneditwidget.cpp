#include "post3dwindownodevectorparticlegroupstructureddataitem_settingeditwidget_startpositioneditwidget.h"
#include "ui_post3dwindownodevectorparticlegroupstructureddataitem_settingeditwidget_startpositioneditwidget.h"

Post3dWindowNodeVectorParticleGroupStructuredDataItem::SettingEditWidget::StartPositionEditWidget::StartPositionEditWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Post3dWindowNodeVectorParticleGroupStructuredDataItem_SettingEditWidget_StartPositionEditWidget)
{
	ui->setupUi(this);
}

Post3dWindowNodeVectorParticleGroupStructuredDataItem::SettingEditWidget::StartPositionEditWidget::~StartPositionEditWidget()
{
	delete ui;
}

void Post3dWindowNodeVectorParticleGroupStructuredDataItem::SettingEditWidget::StartPositionEditWidget::setDimensions(int* dims)
{
	ui->faceWidget->setDimensions(dims);
}

Post3dWindowNodeVectorParticleGroupStructuredDataItem::Setting::StartPosition Post3dWindowNodeVectorParticleGroupStructuredDataItem::SettingEditWidget::StartPositionEditWidget::setting() const
{
	Setting::StartPosition ret;
	ret.face = ui->faceWidget->setting();
	ret.skipOrSubdivide = ui->skipWidget->setting();
	ret.color = ui->colorWidget->color();
	ret.pointSize = ui->pointSizeSpinBox->value();

	return ret;
}

void Post3dWindowNodeVectorParticleGroupStructuredDataItem::SettingEditWidget::StartPositionEditWidget::setSetting(const Setting::StartPosition& pos)
{
	ui->faceWidget->setSetting(pos.face);
	ui->skipWidget->setSetting(pos.skipOrSubdivide);
	ui->colorWidget->setColor(pos.color);
	ui->pointSizeSpinBox->setValue(pos.pointSize);
}
