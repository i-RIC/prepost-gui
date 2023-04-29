#include "valueselectdialog.h"
#include "ui_valueselectdialog.h"

#include <map>

ValueSelectDialog::ValueSelectDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ValueSelectDialog)
{
	ui->setupUi(this);
}

ValueSelectDialog::~ValueSelectDialog()
{
	delete ui;
}

void ValueSelectDialog::setValues(const std::unordered_map<std::string, QString>& values)
{
	std::map<QString, std::string> reverseMap;

	for (const auto& pair : values) {
		reverseMap.insert({pair.second, pair.first});
	}

	for (const auto& pair : reverseMap) {
		ui->valueComboBox->addItem(pair.first);
		m_values.push_back(pair.second);
	}
}

std::string ValueSelectDialog::selectedValue() const
{
	return m_values.at(ui->valueComboBox->currentIndex());
}
