#include "../post3dwindowgridtypedataitem.h"
#include "post3dwindownodescalargroupdataitem_impl.h"
#include "post3dwindownodescalargroupdataitem_setting.h"
#include "post3dwindownodescalargroupdataitem_settingeditwidget.h"
#include "ui_post3dwindownodescalargroupdataitem_settingeditwidget.h"

#include <guibase/comboboxtool.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/grid/v4structured3dgrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <misc/stringtool.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

Post3dWindowNodeScalarGroupDataItem::SettingEditWidget::SettingEditWidget(Post3dWindowNodeScalarGroupDataItem* item, QWidget *parent) :
	ModifyCommandWidget(parent),
	m_item {item},
	ui(new Ui::Post3dWindowNodeScalarGroupDataItem_SettingEditWidget)
{
	ui->setupUi(this);

	connect<void (QComboBox::*)(int)> (ui->physicalValueComboBox, &QComboBox::currentIndexChanged, this, &SettingEditWidget::targetChanged);
	connect(ui->fullRangeCheckBox, &QCheckBox::clicked, this, &SettingEditWidget::fullRangeChanged);
	connect(ui->iminSlider, &SliderWithValue::valueChanged, this, &SettingEditWidget::iMinChanged);
	connect(ui->imaxSlider, &SliderWithValue::valueChanged, this, &SettingEditWidget::iMaxChanged);
	connect(ui->jminSlider, &SliderWithValue::valueChanged, this, &SettingEditWidget::jMinChanged);
	connect(ui->jmaxSlider, &SliderWithValue::valueChanged, this, &SettingEditWidget::jMaxChanged);
	connect(ui->kminSlider, &SliderWithValue::valueChanged, this, &SettingEditWidget::kMinChanged);
	connect(ui->kmaxSlider, &SliderWithValue::valueChanged, this, &SettingEditWidget::kMaxChanged);
}

Post3dWindowNodeScalarGroupDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

void Post3dWindowNodeScalarGroupDataItem::SettingEditWidget::setGridTypeDataItem(Post3dWindowGridTypeDataItem* item)
{
	m_gridTypeDataItem = item;
}

void Post3dWindowNodeScalarGroupDataItem::SettingEditWidget::setZoneData(v4PostZoneDataContainer* zoneData)
{
	vtkPointData* pd = zoneData->gridData()->grid()->vtkData()->data()->GetPointData();

	m_targets = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(pd);
	ComboBoxTool::setupItems(m_gridTypeDataItem->gridType()->outputCaptions(m_targets), ui->physicalValueComboBox);

	auto grid = dynamic_cast<v4Structured3dGrid*> (zoneData->gridData()->grid())->vtkConcreteData()->concreteData();
	int dims[3];
	grid->GetDimensions(dims);
	ui->iminSlider->setRange(1, dims[0]);
	ui->imaxSlider->setRange(1, dims[0]);
	ui->jminSlider->setRange(1, dims[1]);
	ui->jmaxSlider->setRange(1, dims[1]);
	ui->kminSlider->setRange(1, dims[2]);
	ui->kmaxSlider->setRange(1, dims[2]);
}

Post3dWindowNodeScalarGroupDataItem::Setting Post3dWindowNodeScalarGroupDataItem::SettingEditWidget::setting() const
{
	Setting setting;

	setting.target = m_targets.at(ui->physicalValueComboBox->currentIndex()).c_str();
	setting.fullRange = ui->fullRangeCheckBox->isChecked();

	setting.range.iMin = ui->iminSlider->value() - 1;
	setting.range.iMax = ui->imaxSlider->value() - 1;
	setting.range.jMin = ui->jminSlider->value() - 1;
	setting.range.jMax = ui->jmaxSlider->value() - 1;
	setting.range.kMin = ui->kminSlider->value() - 1;
	setting.range.kMax = ui->kmaxSlider->value() - 1;

	setting.isoValue = ui->isoValueEdit->value();
	setting.color = ui->colorEditWidget->color();
	setting.opacity = ui->opacityWidget->opacity();

	return setting;
}

void Post3dWindowNodeScalarGroupDataItem::SettingEditWidget::setSetting(const Setting& setting)
{
	auto it = std::find(m_targets.begin(), m_targets.end(), iRIC::toStr(setting.target));
	if (it == m_targets.end()) {it = m_targets.begin();}
	ui->physicalValueComboBox->setCurrentIndex(it - m_targets.begin());
	targetChanged(it - m_targets.begin());

	ui->fullRangeCheckBox->setChecked(setting.fullRange);

	ui->iminSlider->setValue(setting.range.iMin + 1);
	ui->imaxSlider->setValue(setting.range.iMax + 1);
	ui->jminSlider->setValue(setting.range.jMin + 1);
	ui->jmaxSlider->setValue(setting.range.jMax + 1);
	ui->kminSlider->setValue(setting.range.kMin + 1);
	ui->kmaxSlider->setValue(setting.range.kMax + 1);

	ui->isoValueEdit->setValue(setting.isoValue);
	ui->colorEditWidget->setColor(setting.color);
	ui->opacityWidget->setOpacity(setting.opacity);
}

QUndoCommand* Post3dWindowNodeScalarGroupDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	return new ValueModifyCommmand<Setting>(iRIC::generateCommandId("Post3dWindowNodeScalarGroupDataItem::SettingEditWidget"), apply, setting(), &m_item->impl->m_setting);
}

void Post3dWindowNodeScalarGroupDataItem::SettingEditWidget::targetChanged(int index)
{
	std::string target = m_targets.at(index);
	auto range = m_gridTypeDataItem->nodeValueRange(target);

	ui->minValueEdit->setValue(range.minValue);
	ui->maxValueEdit->setValue(range.maxValue);
}

void Post3dWindowNodeScalarGroupDataItem::SettingEditWidget::fullRangeChanged(bool full)
{
	if (full) {
		ui->iminSlider->setValue(1);
		ui->imaxSlider->setValue(ui->imaxSlider->maximum());
		ui->jminSlider->setValue(1);
		ui->jmaxSlider->setValue(ui->jmaxSlider->maximum());
		ui->kminSlider->setValue(1);
		ui->kmaxSlider->setValue(ui->kmaxSlider->maximum());
	}
}

void Post3dWindowNodeScalarGroupDataItem::SettingEditWidget::iMinChanged(int min)
{
	if (ui->imaxSlider->value() < min) {
		ui->imaxSlider->setValue(min);
	}
}

void Post3dWindowNodeScalarGroupDataItem::SettingEditWidget::iMaxChanged(int max)
{
	if (ui->iminSlider->value() > max) {
		ui->iminSlider->setValue(max);
	}
}

void Post3dWindowNodeScalarGroupDataItem::SettingEditWidget::jMinChanged(int min)
{
	if (ui->jmaxSlider->value() < min) {
		ui->jmaxSlider->setValue(min);
	}
}

void Post3dWindowNodeScalarGroupDataItem::SettingEditWidget::jMaxChanged(int max)
{
	if (ui->jminSlider->value() > max) {
		ui->jminSlider->setValue(max);
	}
}

void Post3dWindowNodeScalarGroupDataItem::SettingEditWidget::kMinChanged(int min)
{
	if (ui->kmaxSlider->value() < min) {
		ui->kmaxSlider->setValue(min);
	}
}

void Post3dWindowNodeScalarGroupDataItem::SettingEditWidget::kMaxChanged(int max)
{
	if (ui->kminSlider->value() > max) {
		ui->kminSlider->setValue(max);
	}
}
