#include "graph2dscatteredwindowdrawsettingdialog.h"
#include "ui_graph2dscatteredwindowdrawsettingdialog.h"

Graph2dScatteredWindowDrawSettingDialog::Graph2dScatteredWindowDrawSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Graph2dScatteredWindowDrawSettingDialog)
{
	ui->setupUi(this);

	connect(ui->resultListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(setResultItem(int)));
	connect(ui->resultSettingWidget, SIGNAL(labelChanged(QString)), this, SLOT(updateResultLabel(QString)));
}

Graph2dScatteredWindowDrawSettingDialog::~Graph2dScatteredWindowDrawSettingDialog()
{
	delete ui;
}

void Graph2dScatteredWindowDrawSettingDialog::setResultSettings(const QList<Graph2dScatteredWindowResultSetting::Setting>& list)
{
	m_resultSettings = list;
	ui->resultListWidget->clear();
	for (int i = 0; i < list.count(); ++i) {
		Graph2dScatteredWindowResultSetting::Setting setting = list.at(i);
		ui->resultListWidget->addItem(setting.name());
	}
	ui->resultListWidget->setCurrentRow(0);
}

void Graph2dScatteredWindowDrawSettingDialog::setResultItem(int index)
{
	Graph2dScatteredWindowResultSetting::Setting* s = &(m_resultSettings[index]);
	ui->resultSettingWidget->setSetting(s);
}

void Graph2dScatteredWindowDrawSettingDialog::updateResultLabel(const QString& label)
{
	ui->resultListWidget->currentItem()->setText(label);
}
