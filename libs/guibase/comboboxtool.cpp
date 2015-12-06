#include "comboboxtool.h"

#include <QComboBox>
#include <QStringList>

void ComboBoxTool::setupItems(const std::vector<std::string>& items, QComboBox* comboBox)
{
	comboBox->blockSignals(true);
	comboBox->clear();
	for (std::string item : items) {
		comboBox->addItem(item.c_str());
	}
	comboBox->blockSignals(false);
}

void ComboBoxTool::setupItems(const QStringList& items, QComboBox* comboBox)
{
	comboBox->blockSignals(true);
	comboBox->clear();
	for (auto item : items) {
		comboBox->addItem(item);
	}
	comboBox->blockSignals(false);
}
