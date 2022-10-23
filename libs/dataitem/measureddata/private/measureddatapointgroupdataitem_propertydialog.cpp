#include "../measureddatafiledataitem.h"
#include "../measureddatapointgroupdataitem.h"
#include "measureddatapointgroupdataitem_impl.h"
#include "measureddatapointgroupdataitem_propertydialog.h"
#include "measureddatapointgroupdataitem_updatesettingcommand.h"
#include "ui_measureddatapointgroupdataitem_propertydialog.h"

#include <guibase/comboboxtool.h>
#include <guicore/project/measured/measureddata.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>

MeasuredDataPointGroupDataItem::PropertyDialog::PropertyDialog(MeasuredDataPointGroupDataItem* item, QWidget *parent) :
	QDialog(parent),
	m_applied {false},
	m_targets {},
	m_item {item},
	ui(new Ui::MeasuredDataPointGroupDataItem_PropertyDialog)
{
	ui->setupUi(this);
	connect<void(QComboBox::*)(int)>(ui->valueComboBox, &QComboBox::currentIndexChanged, this, &MeasuredDataPointGroupDataItem::PropertyDialog::targetChanged);
	connect(ui->buttonBox, &QDialogButtonBox::clicked, this, &PropertyDialog::handleButtonClick);
	connect(ui->importButton, &QPushButton::clicked, [=](bool) {ui->colorMapWidget->importSetting();});
	connect(ui->exportButton, &QPushButton::clicked, [=](bool) {ui->colorMapWidget->exportSetting();});

	auto md = dynamic_cast<MeasuredDataFileDataItem*> (item->parent())->measuredData();

	m_targets = md->scalarNames();
	ComboBoxTool::setupItems(m_targets, ui->valueComboBox);

	ui->surfaceRadioButton->setDisabled(md->noPolyData());
	if (md->noPolyData()) {
		ui->pointsRadioButton->setChecked(true);
	}

	setSetting(item->impl->m_setting);
}

MeasuredDataPointGroupDataItem::PropertyDialog::~PropertyDialog()
{
	delete ui;
}

QUndoCommand* MeasuredDataPointGroupDataItem::PropertyDialog::createModifyCommand()
{
	auto s = setting();

	ColorMapSettingContainer* csp = nullptr;
	auto cs = ui->colorMapWidget->concreteSetting();

	if (s.mappingMode == MeasuredDataPointSetting::MappingMode::Value) {
		csp = &cs;
	}

	return new UpdateSettingCommand(s, csp, m_item);
}

void MeasuredDataPointGroupDataItem::PropertyDialog::accept()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}

	QDialog::accept();
}

void MeasuredDataPointGroupDataItem::PropertyDialog::reject()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
		m_item->renderGraphicsView();
	}

	QDialog::reject();
}

void MeasuredDataPointGroupDataItem::PropertyDialog::targetChanged(int index)
{
	auto newvalue = m_targets.at(index);
	auto s = m_item->impl->m_colorMapSettings.at(newvalue);
	ui->colorMapWidget->setConcreteSetting(*s);
}

void MeasuredDataPointGroupDataItem::PropertyDialog::handleButtonClick(QAbstractButton* button)
{
	if (ui->buttonBox->buttonRole(button) == QDialogButtonBox::ApplyRole) {
		apply();
	}
}

MeasuredDataPointSetting MeasuredDataPointGroupDataItem::PropertyDialog::setting() const
{
	MeasuredDataPointSetting ret;

	if (ui->pointsRadioButton->isChecked()) {
		ret.shapeMode = MeasuredDataPointSetting::ShapeMode::Points;
	} else if (ui->surfaceRadioButton->isChecked()) {
		ret.shapeMode = MeasuredDataPointSetting::ShapeMode::Surface;
	}
	if (ui->customColorRadioButton->isChecked()) {
		ret.mappingMode = MeasuredDataPointSetting::MappingMode::Arbitrary;
	} else if (ui->valueRadioButton->isChecked()) {
		ret.mappingMode = MeasuredDataPointSetting::MappingMode::Value;
	}
	ret.color = ui->colorEdit->color();

	if (m_targets.size() > 0) {
		ret.value = m_targets.at(ui->valueComboBox->currentIndex()).c_str();
	}
	ret.opacity = ui->transparencyWidget->opacityPercent();
	ret.pointSize = ui->pointSizeSpinBox->value();

	return ret;
}

void MeasuredDataPointGroupDataItem::PropertyDialog::setSetting(const MeasuredDataPointSetting& setting)
{
	if (setting.shapeMode == MeasuredDataPointSetting::ShapeMode::Points) {
		ui->pointsRadioButton->setChecked(true);
	} else if (setting.shapeMode == MeasuredDataPointSetting::ShapeMode::Surface) {
		ui->surfaceRadioButton->setChecked(true);
	}
	if (setting.mappingMode == MeasuredDataPointSetting::MappingMode::Arbitrary) {
		ui->customColorRadioButton->setChecked(true);
	} else if (setting.mappingMode == MeasuredDataPointSetting::MappingMode::Value) {
		ui->valueRadioButton->setChecked(true);
	}
	ui->colorEdit->setColor(setting.color);
	if (m_targets.size() > 0) {
		for (int i = 0; i < m_targets.size(); ++i) {
			auto t = m_targets.at(i);
			if (setting.value == t.c_str()) {
				ui->valueComboBox->setCurrentIndex(i);
				targetChanged(i);
			}
		}
	}
	ui->transparencyWidget->setOpacityPercent(setting.opacity);
	ui->pointSizeSpinBox->setValue(setting.pointSize);
}

void MeasuredDataPointGroupDataItem::PropertyDialog::apply()
{
	if (m_applied) {
		iRICUndoStack::instance().undo();
	}

	m_item->pushRenderRedoOnlyCommand(createModifyCommand(), m_item, true);
	m_applied = true;
}
