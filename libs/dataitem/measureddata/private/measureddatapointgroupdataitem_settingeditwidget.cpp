#include "../measureddatafiledataitem.h"
#include "measureddatapointgroupdataitem_impl.h"
#include "measureddatapointgroupdataitem_settingeditwidget.h"
#include "measureddatapointgroupdataitem_updatesettingcommand.h"
#include "ui_measureddatapointgroupdataitem_settingeditwidget.h"

#include <guibase/comboboxtool.h>
#include <guicore/project/measured/measureddata.h>
#include <guicore/scalarstocolors/colormapsettingeditwidget.h>
#include <guicore/scalarstocolors/colormapsettingeditwidgetwithimportexportbutton.h>

MeasuredDataPointGroupDataItem::SettingEditWidget::SettingEditWidget(MeasuredDataPointGroupDataItem* item, QWidget *parent) :
	ModifyCommandWidget {parent},
	m_targets {},
	m_colorMapWidget {new ColorMapSettingEditWidget(this)},
	m_item {item},
	ui(new Ui::MeasuredDataPointGroupDataItem_SettingEditWidget)
{
	ui->setupUi(this);
	connect<void(QComboBox::*)(int)>(ui->valueComboBox, &QComboBox::currentIndexChanged, this, &SettingEditWidget::targetChanged);

	auto w = new ColorMapSettingEditWidgetWithImportExportButton(m_colorMapWidget, this);
	ui->colorMapWidget->setWidget(w);

	auto md = dynamic_cast<MeasuredDataFileDataItem*> (item->parent())->measuredData();
	m_targets = md->scalarNames();
	ComboBoxTool::setupItems(m_targets, ui->valueComboBox);

	ui->surfaceRadioButton->setDisabled(md->noPolyData());
	if (md->noPolyData()) {
		ui->pointsRadioButton->setChecked(true);
	}

	setSetting(item->impl->m_setting);
}

MeasuredDataPointGroupDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

QUndoCommand* MeasuredDataPointGroupDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	auto s = setting();

	ColorMapSettingContainer* csp = nullptr;
	auto cs = m_colorMapWidget->concreteSetting();

	if (s.mappingMode == Setting::MappingMode::Value) {
		csp = &cs;
	}

	return new UpdateSettingCommand(apply, s, csp, m_item);
}

void MeasuredDataPointGroupDataItem::SettingEditWidget::targetChanged(int index)
{
	auto newvalue = m_targets.at(index);
	auto s = dynamic_cast<ColorMapSettingContainer*> (m_item->impl->m_colorMapSettings.at(newvalue));
	m_colorMapWidget->setConcreteSetting(*s);
}

MeasuredDataPointGroupDataItem::Setting MeasuredDataPointGroupDataItem::SettingEditWidget::setting() const
{
	Setting ret;

	if (ui->pointsRadioButton->isChecked()) {
		ret.shapeMode = Setting::ShapeMode::Points;
	} else if (ui->surfaceRadioButton->isChecked()) {
		ret.shapeMode = Setting::ShapeMode::Surface;
	}
	if (ui->customColorRadioButton->isChecked()) {
		ret.mappingMode = Setting::MappingMode::Arbitrary;
	} else if (ui->valueRadioButton->isChecked()) {
		ret.mappingMode = Setting::MappingMode::Value;
	}
	ret.color = ui->colorEdit->color();

	if (m_targets.size() > 0) {
		ret.value = m_targets.at(ui->valueComboBox->currentIndex()).c_str();
	}
	ret.opacity = ui->transparencyWidget->opacity();
	ret.pointSize = ui->pointSizeSpinBox->value();

	return ret;
}

void MeasuredDataPointGroupDataItem::SettingEditWidget::setSetting(const Setting& setting)
{
	if (setting.shapeMode == Setting::ShapeMode::Points) {
		ui->pointsRadioButton->setChecked(true);
	} else if (setting.shapeMode == Setting::ShapeMode::Surface) {
		ui->surfaceRadioButton->setChecked(true);
	}
	if (setting.mappingMode == Setting::MappingMode::Arbitrary) {
		ui->customColorRadioButton->setChecked(true);
	} else if (setting.mappingMode == Setting::MappingMode::Value) {
		ui->valueRadioButton->setChecked(true);
	}
	ui->colorEdit->setColor(setting.color);
	if (m_targets.size() > 0) {
		bool found = false;
		for (int i = 0; i < m_targets.size(); ++i) {
			auto t = m_targets.at(i);
			if (setting.value == t.c_str()) {
				ui->valueComboBox->setCurrentIndex(i);
				targetChanged(i);
				found = true;
			}
		}
		if (! found) {
			ui->valueComboBox->setCurrentIndex(0);
			targetChanged(0);
		}
	}
	ui->transparencyWidget->setOpacity(setting.opacity);
	ui->pointSizeSpinBox->setValue(setting.pointSize);
}
