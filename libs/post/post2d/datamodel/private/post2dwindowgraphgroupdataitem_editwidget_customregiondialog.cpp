#include "post2dwindowgraphgroupdataitem_editwidget_customregiondialog.h"
#include "ui_post2dwindowgraphgroupdataitem_editwidget_customregiondialog.h"

Post2dWindowGraphGroupDataItem::EditWidget::CustomRegionDialog::CustomRegionDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Post2dWindowGraphGroupDataItem_EditWidget_CustomRegionDialog)
{
	ui->setupUi(this);

	connect(ui->selectAllPushButton, &QPushButton::clicked, this, &CustomRegionDialog::selectAll);
	connect(ui->unselectAllPushButton, &QPushButton::clicked, this, &CustomRegionDialog::unselectAll);
}

Post2dWindowGraphGroupDataItem::EditWidget::CustomRegionDialog::~CustomRegionDialog()
{
	delete ui;
}

void Post2dWindowGraphGroupDataItem::EditWidget::CustomRegionDialog::setIndexCount(int count)
{
	ui->listWidget->clear();

	for (int i = 0; i < count; ++i) {
		ui->listWidget->addItem(QString::number(i + 1));
	}
}

std::vector<int> Post2dWindowGraphGroupDataItem::EditWidget::CustomRegionDialog::setting() const
{
	std::vector<int> ret;

	for (int i = 0; i < ui->listWidget->count(); ++i) {
		QListWidgetItem* item = ui->listWidget->item(i);
		if (item->isSelected()) {
			ret.push_back(i);
		}
	}

	return ret;
}

void Post2dWindowGraphGroupDataItem::EditWidget::CustomRegionDialog::setSetting(const std::vector<int>& setting)
{
	ui->listWidget->clearSelection();

	for (int index : setting) {
		QListWidgetItem* item = ui->listWidget->item(index);
		if (item == nullptr) {continue;}
		item->setSelected(true);
	}
}

void Post2dWindowGraphGroupDataItem::EditWidget::CustomRegionDialog::selectAll()
{
	for (int i = 0; i < ui->listWidget->count(); ++i) {
		ui->listWidget->item(i)->setSelected(true);
	}
}

void Post2dWindowGraphGroupDataItem::EditWidget::CustomRegionDialog::unselectAll()
{
	ui->listWidget->clearSelection();
}
