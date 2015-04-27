#include "itemselectingdialog.h"
#include "ui_itemselectingdialog.h"

ItemSelectingDialog::ItemSelectingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::ItemSelectingDialog)
{
	ui->setupUi(this);
}

ItemSelectingDialog::~ItemSelectingDialog()
{
	delete ui;
}

void ItemSelectingDialog::setMessage(const QString& message)
{
	ui->messageLabel->setText(message);
}

void ItemSelectingDialog::setItems(const QList<QString>& items)
{
	ui->comboBox->clear();
	for (auto it = items.begin(); it != items.end(); ++it) {
		ui->comboBox->addItem(*it);
	}
	ui->comboBox->setCurrentIndex(0);
}

void ItemSelectingDialog::setSelectIndex(int index)
{
	ui->comboBox->setCurrentIndex(index);
}

int ItemSelectingDialog::selectIndex()
{
	return ui->comboBox->currentIndex();
}
