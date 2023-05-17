#include "post2dwindownodevectorparticlegroupunstructureddataitem_settingeditwidget_startpositioneditwidget.h"
#include "ui_post2dwindownodevectorparticlegroupunstructureddataitem_settingeditwidget_startpositioneditwidget.h"

Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::SettingEditWidget::StartPositionEditWidget::StartPositionEditWidget(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::Post2dWindowNodeVectorParticleGroupUnstructuredDataItem_SettingEditWidget_StartPositionEditWidget)
{
	ui->setupUi(this);
}

Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::SettingEditWidget::StartPositionEditWidget::~StartPositionEditWidget()
{
	delete ui;
}

Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting::StartPosition Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::SettingEditWidget::StartPositionEditWidget::setting() const
{
	Setting::StartPosition ret;

	ret.point1 = QPointF(ui->point1XEdit->value(), ui->point1YEdit->value());
	ret.point2 = QPointF(ui->point2XEdit->value(), ui->point2YEdit->value());
	ret.numberOfPoints = ui->numPointsSpinBox->value();
	ret.color = ui->colorWidget->color();
	ret.pointSize = ui->pointSizeSpinBox->value();

	return ret;
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::SettingEditWidget::StartPositionEditWidget::setSetting(const Setting::StartPosition& pos)
{
	ui->point1XEdit->setValue(pos.point1.value().x());
	ui->point1YEdit->setValue(pos.point1.value().y());
	ui->point2XEdit->setValue(pos.point2.value().x());
	ui->point2YEdit->setValue(pos.point2.value().y());
	ui->numPointsSpinBox->setValue(pos.numberOfPoints);
	ui->colorWidget->setColor(pos.color);
	ui->pointSizeSpinBox->setValue(pos.pointSize);
}

bool Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::SettingEditWidget::StartPositionEditWidget::handleMousePoint1Change(const QPointF point)
{
	if (! ui->pointsMouseRadioButton->isChecked()) {return false;}

	ui->point1XEdit->setValue(point.x());
	ui->point1YEdit->setValue(point.y());

	return true;
}

bool Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::SettingEditWidget::StartPositionEditWidget::handleMousePoint2Change(const QPointF point)
{
	if (! ui->pointsMouseRadioButton->isChecked()) {return false;}

	ui->point2XEdit->setValue(point.x());
	ui->point2YEdit->setValue(point.y());

	return true;
}
