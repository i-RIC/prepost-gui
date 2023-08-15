#include "measureddatapointgroupdataitem_impl.h"
#include "measureddatapointgroupdataitem_toolbarwidget.h"
#include "ui_measureddatapointgroupdataitem_toolbarwidget.h"

#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

MeasuredDataPointGroupDataItem::ToolBarWidget::ToolBarWidget(const Setting& setting, MeasuredDataPointGroupDataItem* item, QWidget *parent) :
	QWidget(parent),
	m_item {item},
	ui(new Ui::MeasuredDataPointGroupDataItem_ToolBarWidget)
{
	ui->setupUi(this);
	myApplySetting(setting);

	connect(&setting, &Setting::updated, this, &ToolBarWidget::applySetting);

	connect(ui->colorWidget, &ColorEditWidget::colorChanged, this, &ToolBarWidget::updateActorSetting);
	connect<void (QSpinBox::*)(int)>(ui->pointSizeSpinBox, &QSpinBox::valueChanged, this, &ToolBarWidget::updateActorSetting);
	connect(ui->transparencyWidget, &TransparencyWidget::updated, this, &ToolBarWidget::updateActorSetting);
}

MeasuredDataPointGroupDataItem::ToolBarWidget::~ToolBarWidget()
{
	delete ui;
}

void MeasuredDataPointGroupDataItem::ToolBarWidget::applySetting()
{
	myApplySetting(m_item->impl->m_setting);
}

void MeasuredDataPointGroupDataItem::ToolBarWidget::updateActorSetting()
{
	auto com = new ValueModifyCommmand<Setting>(iRIC::generateCommandId("ModifyToolBar"), false,
																										 modifiedSetting(), &m_item->impl->m_setting);
	m_item->pushUpdateActorSettingCommand(com, m_item);
}

void MeasuredDataPointGroupDataItem::ToolBarWidget::myApplySetting(const Setting& setting)
{
	const auto& s = setting;
	ui->colorWidget->setEnabled(s.mappingMode == Setting::MappingMode::Arbitrary);
	ui->colorWidget->setColor(s.color);

	ui->pointSizeSpinBox->blockSignals(true);
	ui->pointSizeSpinBox->setValue(s.pointSize);
	ui->pointSizeSpinBox->blockSignals(false);

	ui->transparencyWidget->blockSignals(true);
	ui->transparencyWidget->setOpacity(s.opacity);
	ui->transparencyWidget->blockSignals(false);
}

MeasuredDataPointGroupDataItem::Setting MeasuredDataPointGroupDataItem::ToolBarWidget::modifiedSetting() const
{
	Setting ret = m_item->impl->m_setting;

	ret.color = ui->colorWidget->color();
	ret.pointSize = ui->pointSizeSpinBox->value();
	ret.opacity = ui->transparencyWidget->opacity();

	return ret;
}
