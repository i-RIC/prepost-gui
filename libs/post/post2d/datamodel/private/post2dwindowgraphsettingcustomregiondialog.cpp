#include "post2dwindowgraphsettingcustomregiondialog.h"
#include "ui_post2dwindowgraphsettingcustomregiondialog.h"

Post2dWindowGraphSettingCustomRegionDialog::Post2dWindowGraphSettingCustomRegionDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Post2dWindowGraphSettingCustomRegionDialog)
{
	ui->setupUi(this);

	connect(ui->selectAllPushButton, SIGNAL(clicked(bool)), this, SLOT(selectAll()));
	connect(ui->unselectAllPushButton, SIGNAL(clicked(bool)), this, SLOT(unselectAll()));
}

Post2dWindowGraphSettingCustomRegionDialog::~Post2dWindowGraphSettingCustomRegionDialog()
{
	delete ui;
}

void Post2dWindowGraphSettingCustomRegionDialog::setIndexCount(int count)
{
	ui->listWidget->clear();

	for (int i = 0; i < count; ++i) {
		ui->listWidget->addItem(QString::number(i + 1));
	}
}

std::vector<int> Post2dWindowGraphSettingCustomRegionDialog::setting() const
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

void Post2dWindowGraphSettingCustomRegionDialog::setSetting(const std::vector<int>& setting)
{
	ui->listWidget->clearSelection();;

	for (int index : setting) {
		QListWidgetItem* item = ui->listWidget->item(index);
		if (item == nullptr) {continue;}
		item->setSelected(true);
	}
}

void Post2dWindowGraphSettingCustomRegionDialog::selectAll()
{
	for (int i = 0; i < ui->listWidget->count(); ++i) {
		ui->listWidget->item(i)->setSelected(true);
	}
}

void Post2dWindowGraphSettingCustomRegionDialog::unselectAll()
{
	ui->listWidget->clearSelection();
}
