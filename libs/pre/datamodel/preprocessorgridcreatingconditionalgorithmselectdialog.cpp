#include "preprocessorgridcreatingconditionalgorithmselectdialog.h"
#include "ui_preprocessorgridcreatingconditionalgorithmselectdialog.h"
#include <guicore/pre/gridcreatingcondition/gridcreatingconditioncreator.h>
#include <QWidget>

#define LISTWIDTH 280
#define CAPTION_PADDING 10
#define MARGINS 30

PreProcessorGridCreatingConditionAlgorithmSelectDialog::PreProcessorGridCreatingConditionAlgorithmSelectDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PreProcessorGridCreatingConditionAlgorithmSelectDialog)
{
	ui->setupUi(this);
	ui->splitter->setStretchFactor(0, 2);
	ui->splitter->setStretchFactor(1, 5);
	connect(ui->algoListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(handleCurrentChange(int)));
	connect(ui->algoListWidget, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(accept()));

}

PreProcessorGridCreatingConditionAlgorithmSelectDialog::~PreProcessorGridCreatingConditionAlgorithmSelectDialog()
{
	delete ui;
}

void PreProcessorGridCreatingConditionAlgorithmSelectDialog::setCreators(const QList<GridCreatingConditionCreator*>& creators)
{
	m_creators = creators;
	ui->algoListWidget->clear();
	int row = 0;
	for (auto it = m_creators.begin(); it != m_creators.end(); ++it){
		GridCreatingConditionCreator* creator = *it;
		ui->algoListWidget->addItem(creator->caption());
		++ row;
	}
	QList<int> sizes;
	int width = LISTWIDTH;
	sizes.append(width);
	sizes.append(size().width() - LISTWIDTH - MARGINS);
	ui->splitter->setSizes(sizes);
}

void PreProcessorGridCreatingConditionAlgorithmSelectDialog::setCurrent(GridCreatingConditionCreator* creator)
{
	int index = m_creators.indexOf(creator);
	ui->algoListWidget->setCurrentRow(index);
}

void PreProcessorGridCreatingConditionAlgorithmSelectDialog::handleCurrentChange(int index)
{
	GridCreatingConditionCreator* creator = m_creators.at(index);
	ui->descBrowser->setText(creator->description());
}

GridCreatingConditionCreator* PreProcessorGridCreatingConditionAlgorithmSelectDialog::selectedCreator()
{
	int index = ui->algoListWidget->currentRow();
	return m_creators.at(index);
}

