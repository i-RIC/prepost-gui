#include "post3dwindowparticlesbasescalargroupdataitem_toolbarwidget.h"
#include "ui_post3dwindowparticlesbasescalargroupdataitem_toolbarwidget.h"

#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

Post3dWindowParticlesBaseScalarGroupDataItem::ToolBarWidget::ToolBarWidget(Post3dWindowParticlesBaseScalarGroupDataItem* item, QWidget *parent) :
	QWidget(parent),
	m_item {item},
	ui(new Ui::Post3dWindowParticlesBaseScalarGroupDataItem_ToolBarWidget)
{
	ui->setupUi(this);
	applySetting();

	connect(&m_item->m_setting, &ParticleDataSetting::updated, this, &ToolBarWidget::applySetting);

	connect(ui->colorWidget, &ColorEditWidget::colorChanged, this, &ToolBarWidget::updateActorSetting);
	connect<void (QSpinBox::*)(int)>(ui->pointSizeSpinBox, &QSpinBox::valueChanged, this, &ToolBarWidget::updateActorSetting);
	connect(ui->transparencyWidget, &TransparencyWidget::updated, this, &ToolBarWidget::updateActorSetting);
}

Post3dWindowParticlesBaseScalarGroupDataItem::ToolBarWidget::~ToolBarWidget()
{
	delete ui;
}

void Post3dWindowParticlesBaseScalarGroupDataItem::ToolBarWidget::applySetting()
{
	const auto& s = m_item->m_setting;
	ui->colorWidget->setEnabled(s.mapping == ParticleDataSetting::Mapping::Arbitrary);
	ui->colorWidget->setColor(s.color);

	ui->pointSizeSpinBox->blockSignals(true);
	ui->pointSizeSpinBox->setValue(s.particleSize);
	ui->pointSizeSpinBox->blockSignals(false);

	ui->transparencyWidget->blockSignals(true);
	ui->transparencyWidget->setOpacity(s.opacity);
	ui->transparencyWidget->blockSignals(false);
}

void Post3dWindowParticlesBaseScalarGroupDataItem::ToolBarWidget::updateActorSetting()
{
	auto com = new ValueModifyCommmand<ParticleDataSetting>(iRIC::generateCommandId("ModifyToolBar"), false,
																										 modifiedSetting(), &m_item->m_setting);
	m_item->pushUpdateActorSettingCommand(com, m_item);
}

ParticleDataSetting Post3dWindowParticlesBaseScalarGroupDataItem::ToolBarWidget::modifiedSetting() const
{
	ParticleDataSetting ret = m_item->m_setting;

	ret.color = ui->colorWidget->color();
	ret.particleSize = ui->pointSizeSpinBox->value();
	ret.opacity = ui->transparencyWidget->opacity();

	return ret;
}
