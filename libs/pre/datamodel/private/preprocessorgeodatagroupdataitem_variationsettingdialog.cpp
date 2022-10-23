#include "preprocessorgeodatagroupdataitem_variationsettingdialog.h"
#include "ui_preprocessorgeodatagroupdataitem_variationsettingdialog.h"

#include <misc/stringtool.h>

PreProcessorGeoDataGroupDataItem::VariationSettingDialog::VariationSettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PreProcessorGeoDataGroupDataItem_VariationSettingDialog)
{
	ui->setupUi(this);
}

PreProcessorGeoDataGroupDataItem::VariationSettingDialog::~VariationSettingDialog()
{
	delete ui;
}

void PreProcessorGeoDataGroupDataItem::VariationSettingDialog::setVariations(const std::map<std::string, QString> variations)
{
	m_variations.clear();
	ui->activeVariationComboBox->clear();

	for (const auto& pair : variations) {
		m_variations.push_back(pair.first);
		ui->activeVariationComboBox->addItem(pair.second);
	}
}

PreProcessorGeoDataGroupDataItem::VariationSetting PreProcessorGeoDataGroupDataItem::VariationSettingDialog::setting() const
{
	VariationSetting setting;

	setting.enabled = ui->enableCheckBox->isChecked();
	setting.activeVariation = m_variations.at(ui->activeVariationComboBox->currentIndex()).c_str();

	return setting;
}

void PreProcessorGeoDataGroupDataItem::VariationSettingDialog::setSetting(const VariationSetting& setting)
{
	ui->enableCheckBox->setChecked(setting.enabled);
	auto v = iRIC::toStr(setting.activeVariation);
	for (int i = 0; i < m_variations.size(); ++i) {
		if (m_variations.at(i) == v) {
			ui->activeVariationComboBox->setCurrentIndex(i);
		}
	}
}
