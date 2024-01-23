#include "orderedvalueselectdialog.h"
#include "ui_orderedvalueselectdialog.h"

#include <map>

OrderedValueSelectDialog::OrderedValueSelectDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::OrderedValueSelectDialog)
{
	ui->setupUi(this);
}

OrderedValueSelectDialog::~OrderedValueSelectDialog()
{
	delete ui;
}

void OrderedValueSelectDialog::setValues(const std::vector<std::string>& values, const std::unordered_map<std::string, QString>& captions)
{
	m_values = values;

	for (const auto& v : values) {
		QString c = v.c_str();
		auto it = captions.find(v);
		if (it != captions.end()) {
			c = it->second;
		}

		ui->valueComboBox->addItem(c);
	}
}

std::string OrderedValueSelectDialog::selectedValue() const
{
	return m_values.at(ui->valueComboBox->currentIndex());
}
