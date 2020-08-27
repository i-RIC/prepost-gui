#include "itemmultiselectingdialog.h"
#include "ui_itemmultiselectingdialog.h"

#include <QCheckBox>
#include <QVBoxLayout>

ItemMultiSelectingDialog::ItemMultiSelectingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ItemMultiSelectingDialog)
{
	ui->setupUi(this);
	connect(ui->checkAllButton, SIGNAL(clicked()), this, SLOT(checkAll()));
}

ItemMultiSelectingDialog::~ItemMultiSelectingDialog()
{
	delete ui;
}

void ItemMultiSelectingDialog::setItems(const std::vector<QString>& items)
{
	auto w = ui->scrollAreaWidgetContents;
	auto layout = new QVBoxLayout(w);

	for (auto item : items) {
		auto checkBox = new QCheckBox(item, w);
		m_checkBoxes.push_back(checkBox);
		layout->addWidget(checkBox);
	}
	layout->addStretch();
	w->setLayout(layout);
}

std::vector<bool> ItemMultiSelectingDialog::selectSettings() const
{
	std::vector<bool> ret;
	for (auto checkBox : m_checkBoxes) {
		ret.push_back(checkBox->isChecked());
	}
	return ret;
}

void ItemMultiSelectingDialog::checkAll()
{
	for (auto cb : m_checkBoxes) {
		cb->setChecked(true);
	}
}
