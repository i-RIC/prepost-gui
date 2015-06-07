#include "ui_graph2dhybridwindowdrawsettingdialog.h"

#include "graph2dhybridwindowdrawsettingdialog.h"

Graph2dHybridWindowDrawSettingDialog::Graph2dHybridWindowDrawSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Graph2dHybridWindowDrawSettingDialog)
{
	ui->setupUi(this);
	ui->resultSettingWidget->setupForResult();
	ui->copySettingWidget->setupForResultCopy();
	ui->extSettingWidget->setupForExternalData();

	connect(ui->resultListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(setResultItem(int)));
	connect(ui->copyListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(setCopyItem(int)));
	connect(ui->extListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(setImportDataItem(int)));

	connect(ui->resultSettingWidget, SIGNAL(labelChanged(QString)), this, SLOT(updateResultLabel(QString)));
	connect(ui->copySettingWidget, SIGNAL(labelChanged(QString)), this, SLOT(updateCopyLabel(QString)));
	connect(ui->copySettingWidget, SIGNAL(deleteButtonClicked()), this, SLOT(deleteCopyItem()));
	connect(ui->extSettingWidget, SIGNAL(labelChanged(QString)), this, SLOT(updateImportDataLabel(QString)));
	connect(ui->extSettingWidget, SIGNAL(deleteButtonClicked()), this, SLOT(deleteImportDataItem()));
}

Graph2dHybridWindowDrawSettingDialog::~Graph2dHybridWindowDrawSettingDialog()
{
	delete ui;
}

void Graph2dHybridWindowDrawSettingDialog::setResultSettings(const QList<Graph2dHybridWindowResultSetting::Setting>& list)
{
	m_resultSettings = list;
	ui->resultListWidget->clear();
	for (int i = 0; i < list.count(); ++i) {
		Graph2dHybridWindowResultSetting::Setting setting = list.at(i);
		ui->resultListWidget->addItem(setting.name());
	}
	ui->resultListWidget->setCurrentRow(0);
}

void Graph2dHybridWindowDrawSettingDialog::setCopySettings(const QList<Graph2dHybridWindowResultSetting::Setting>& list, QList<QString> ids)
{
	m_copySettings = list;
	m_copyIds = ids;
	m_copyDeleted.clear();
	for (int i = 0; i < ids.count(); ++i) {
		m_copyDeleted.append(false);
	}
	ui->copyListWidget->clear();
	for (int i = 0; i < list.count(); ++i) {
		Graph2dHybridWindowResultSetting::Setting setting = list.at(i);
		ui->copyListWidget->addItem(setting.name());
	}
	if (list.count() > 0) {
		ui->copyListWidget->setCurrentRow(0);
	} else {
		ui->copySettingWidget->setDisabled(true);
	}
}

void Graph2dHybridWindowDrawSettingDialog::setImportDataSettings(const QList<Graph2dHybridWindowResultSetting::Setting>& list, QList<QString> ids)
{
	m_importDataSettings = list;
	m_importDataIds = ids;
	m_importDataDeleted.clear();
	for (int i = 0; i < ids.count(); ++i) {
		m_importDataDeleted.append(false);
	}
	ui->extListWidget->clear();
	for (int i = 0; i < list.count(); ++i) {
		Graph2dHybridWindowResultSetting::Setting setting = list.at(i);
		ui->extListWidget->addItem(setting.name());
	}
	if (list.count() > 0) {
		ui->extListWidget->setCurrentRow(0);
	} else {
		ui->extSettingWidget->setDisabled(true);
	}
}

void Graph2dHybridWindowDrawSettingDialog::setResultItem(int index)
{
	Graph2dHybridWindowResultSetting::Setting* s = &(m_resultSettings[index]);
	ui->resultSettingWidget->setSetting(s);
}

void Graph2dHybridWindowDrawSettingDialog::setCopyItem(int index)
{
	int cindex = copyIndex(index);
	Graph2dHybridWindowResultSetting::Setting* s = &(m_copySettings[cindex]);
	ui->copySettingWidget->setSetting(s);
	ui->copySettingWidget->setId(m_copyIds[cindex]);
}

void Graph2dHybridWindowDrawSettingDialog::setImportDataItem(int index)
{
	int cindex = importDataIndex(index);
	Graph2dHybridWindowResultSetting::Setting* s = &(m_importDataSettings[cindex]);
	ui->extSettingWidget->setSetting(s);
	ui->extSettingWidget->setId(m_importDataIds[cindex]);
}

void Graph2dHybridWindowDrawSettingDialog::updateResultLabel(const QString& label)
{
	ui->resultListWidget->currentItem()->setText(label);
}

void Graph2dHybridWindowDrawSettingDialog::updateCopyLabel(const QString& label)
{
	ui->copyListWidget->currentItem()->setText(label);
}

void Graph2dHybridWindowDrawSettingDialog::updateImportDataLabel(const QString& label)
{
	ui->extListWidget->currentItem()->setText(label);
}

void Graph2dHybridWindowDrawSettingDialog::deleteCopyItem()
{
	int current = ui->copyListWidget->currentRow();
	QListWidgetItem* i = ui->copyListWidget->takeItem(ui->copyListWidget->currentRow());
	delete i;

	int index = copyIndex(current);
	m_copyDeleted[index] = true;
	if (ui->copyListWidget->count() == 0) {
		ui->copySettingWidget->clear();
		ui->copySettingWidget->setDisabled(true);
	} else if (current == ui->copyListWidget->count()) {
		ui->copyListWidget->setCurrentRow(current - 1);
	} else {
		ui->copyListWidget->setCurrentRow(current);
	}
}

void Graph2dHybridWindowDrawSettingDialog::deleteImportDataItem()
{
	int current = ui->extListWidget->currentRow();
	QListWidgetItem* i = ui->extListWidget->takeItem(ui->extListWidget->currentRow());
	delete i;

	int index = importDataIndex(current);
	m_importDataDeleted[index] = true;
	if (ui->extListWidget->count() == 0) {
		ui->extSettingWidget->clear();
		ui->extSettingWidget->setDisabled(true);
	} else if (current == ui->extListWidget->count()) {
		ui->extListWidget->setCurrentRow(current - 1);
	} else {
		ui->extListWidget->setCurrentRow(current);
	}
}

int Graph2dHybridWindowDrawSettingDialog::copyIndex(int index)
{
	int count = 0;
	for (int i = 0; i < m_copyDeleted.count(); ++i) {
		if (m_copyDeleted.at(i) == false) {++ count;}
		if (count > index) {return i;}
	}
	return 0;
}

int Graph2dHybridWindowDrawSettingDialog::importDataIndex(int index)
{
	int count = 0;
	for (int i = 0; i < m_importDataDeleted.count(); ++i) {
		if (m_importDataDeleted.at(i) == false) {++ count;}
		if (count > index) {return i;}
	}
	return 0;
}
