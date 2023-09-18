#include "ui_preprocessorgridattributecustommappingdialog.h"

#include "preprocessorbcdataitem.h"
#include "preprocessorbcsettingdataitem.h"
#include "preprocessorgridattributecustommappingdialog.h"
#include "preprocessorgridattributemappingsettingdataitem.h"

#include <guicore/solverdef/solverdefinitiongridattribute.h>

PreProcessorGridAttributeCustomMappingDialog::PreProcessorGridAttributeCustomMappingDialog(QWidget* parent) :
	QDialog {parent},
	ui {new Ui::PreProcessorGridAttributeCustomMappingDialog}
{
	ui->setupUi(this);

	connect(ui->checkAllButton, &QCheckBox::clicked, this, &PreProcessorGridAttributeCustomMappingDialog::checkAll);
	connect(ui->uncheckAllButton, &QCheckBox::clicked, this, &PreProcessorGridAttributeCustomMappingDialog::uncheckAll);
}

PreProcessorGridAttributeCustomMappingDialog::~PreProcessorGridAttributeCustomMappingDialog()
{
	delete ui;
}

void PreProcessorGridAttributeCustomMappingDialog::setSettings(const QList<PreProcessorGridAttributeMappingSettingDataItem*>& atts, const QList<PreProcessorBCSettingDataItem*>& bcs, const PreProcessorCustomMappingSetting& setting)
{
	QVBoxLayout* layout = new QVBoxLayout(this);
	if (atts.size() != 0) {
		QGroupBox* gbox = new QGroupBox(tr("Geographic Data"), this);
		QVBoxLayout* glayout = new QVBoxLayout(gbox);
		for (int i = 0; i < atts.size(); ++i) {
			PreProcessorGridAttributeMappingSettingDataItem* item = atts.at(i);
			QCheckBox* checkBox = new QCheckBox(item->condition()->caption(), gbox);
			if (setting.attSettings.contains(item->condition()->name())) {
				checkBox->setChecked(setting.attSettings.value(item->condition()->name()));
			} else {
				checkBox->setChecked(false);
			}
			m_attMap.insert({item, checkBox});
			glayout->addWidget(checkBox);
		}
		glayout->addStretch(1);
		gbox->setLayout(glayout);
		layout->addWidget(gbox);
	}

	if (bcs.size() != 0) {
		QGroupBox* bbox = new QGroupBox(tr("Boundary Condition Setting"), this);
		QVBoxLayout* blayout = new QVBoxLayout(bbox);
		for (int i = 0; i < bcs.size(); ++i) {
			PreProcessorBCSettingDataItem* item = bcs.at(i);
			QCheckBox* checkBox = new QCheckBox(item->bcDataItem()->caption(), bbox);
			if (setting.bcSettings.contains(item->bcDataItem()->uniqueName())) {
				checkBox->setChecked(setting.bcSettings.value(item->bcDataItem()->uniqueName()));
			} else {
				checkBox->setChecked(false);
			}
			m_bcMap.insert({item, checkBox});
			blayout->addWidget(checkBox);
		}
		blayout->addStretch(1);
		bbox->setLayout(blayout);
		layout->addWidget(bbox);
	}
	layout->addStretch(1);
	ui->scrollAreaWidgetContents->setLayout(layout);

	adjustSize();
}

PreProcessorCustomMappingSetting PreProcessorGridAttributeCustomMappingDialog::setting() const
{
	PreProcessorCustomMappingSetting ret;
	for (const auto& pair : m_attMap) {
		auto item = pair.first;
		auto checkBox = pair.second;
		ret.attSettings.insert(item->condition()->name(), checkBox->isChecked());
	}
	for (const auto& pair : m_bcMap) {
		auto item = pair.first;
		auto checkBox = pair.second;
		ret.bcSettings.insert(item->bcDataItem()->uniqueName(), checkBox->isChecked());
	}
	return ret;
}

void PreProcessorGridAttributeCustomMappingDialog::checkAll()
{
	for (const auto& pair : m_attMap) {
		auto checkBox = pair.second;
		checkBox->setChecked(true);
	}
	for (const auto& pair : m_bcMap) {
		auto checkBox = pair.second;
		checkBox->setChecked(true);
	}
}

void PreProcessorGridAttributeCustomMappingDialog::uncheckAll()
{
	for (const auto& pair : m_attMap) {
		auto checkBox = pair.second;
		checkBox->setChecked(false);
	}
	for (const auto& pair : m_bcMap) {
		auto checkBox = pair.second;
		checkBox->setChecked(false);
	}
}
