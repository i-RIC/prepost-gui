#include "gridcreatingconditiongridcombinesettingdialog.h"
#include "ui_gridcreatingconditiongridcombinesettingdialog.h"

#include <guicore/pre/base/preprocessorgridtypedataiteminterface.h>
#include <guicore/pre/base/preprocessorgridcreatingconditiondataiteminterface.h>
#include <guicore/pre/base/preprocessorgridandgridcreatingconditiondataiteminterface.h>
#include <guicore/pre/grid/structured2dgrid.h>
#include <guicore/pre/base/preprocessorgriddataiteminterface.h>

#include <QPushButton>

GridCreatingConditionGridCombineSettingDialog::GridCreatingConditionGridCombineSettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::GridCreatingConditionGridCombineSettingDialog)
{
	ui->setupUi(this);
}

GridCreatingConditionGridCombineSettingDialog::~GridCreatingConditionGridCombineSettingDialog()
{
	delete ui;
}

void GridCreatingConditionGridCombineSettingDialog::setType(Type type)
{
	switch (type){
	case Bind:
		ui->bindRadioButton->click();
		break;
	case Left:
		ui->leftRadioButton->click();
		break;
	case Right:
		ui->rightRadioButton->click();
		break;
	}
}

void GridCreatingConditionGridCombineSettingDialog::setMainstreamGridName(const QString &name)
{
	int index = ui->mainstreamComboBox->findText(name);
	ui->mainstreamComboBox->setCurrentIndex(index);
}

void GridCreatingConditionGridCombineSettingDialog::setTributaryGridName(const QString &name)
{
	int index = ui->tributaryComboBox->findText(name);
	ui->tributaryComboBox->setCurrentIndex(index);
}

void GridCreatingConditionGridCombineSettingDialog::setMainstreamCrossingBeginningIndex(int i)
{
	ui->mainstreamCrossingBeginningSpinBox->setValue(i);
}

void GridCreatingConditionGridCombineSettingDialog::setMainstreamCrossingEndingIndex(int i)
{
	ui->mainstreamCrossingEndingSpinBox->setValue(i);
}

void GridCreatingConditionGridCombineSettingDialog::setTributaryCrossingBeginningIndex(int i)
{
	ui->tributaryCrossingBeginningSpinBox->setValue(i);
}

void GridCreatingConditionGridCombineSettingDialog::setTributaryCrossingEndingIndex(int i)
{
	ui->tributaryCrossingEndingSpinBox->setValue(i);
}

void GridCreatingConditionGridCombineSettingDialog::setMainstreamFlowBeginningIndex(int i)
{
	ui->mainstreamFlowBeginningSpinBox->setValue(i);
}

void GridCreatingConditionGridCombineSettingDialog::setMainstreamFlowEndingIndex(int i)
{
	ui->mainstreamFlowEndingSpinBox->setValue(i);
}

void GridCreatingConditionGridCombineSettingDialog::setObstacleUpstreamX(double x)
{
	ui->upstreamXSpinBox->setValue(x);
}

void GridCreatingConditionGridCombineSettingDialog::setObstacleUpstreamY(double y)
{
	ui->upstreamYSpinBox->setValue(y);
}

void GridCreatingConditionGridCombineSettingDialog::setObstacleDownstreamX(double x)
{
	ui->downstreamXSpinBox->setValue(x);
}

void GridCreatingConditionGridCombineSettingDialog::setObstacleDownstreamY(double y)
{
	ui->downstreamYSpinBox->setValue(y);
}

void GridCreatingConditionGridCombineSettingDialog::setCorrectionValue(double c)
{
	ui->correctionSpinBox->setValue(c);
}

void GridCreatingConditionGridCombineSettingDialog::setObstacleElevation(double e)
{
	ui->elevationSpinBox->setValue(e);
}

GridCreatingConditionGridCombineSettingDialog::Type GridCreatingConditionGridCombineSettingDialog::type()
{
	if (ui->bindRadioButton->isChecked()) return Bind;
	if (ui->leftRadioButton->isChecked()) return Left;
	if (ui->rightRadioButton->isChecked()) return Right;
	// default
	return Left;
}

Grid* GridCreatingConditionGridCombineSettingDialog::mainstreamGrid()
{
	return m_gridMap.value(ui->mainstreamComboBox->currentText());
}

Grid* GridCreatingConditionGridCombineSettingDialog::tributaryGrid()
{
	return m_gridMap.value(ui->tributaryComboBox->currentText());
}

QString GridCreatingConditionGridCombineSettingDialog::mainstreamGridName()
{
	return ui->mainstreamComboBox->currentText();
}

QString GridCreatingConditionGridCombineSettingDialog::tributaryGridName()
{
	return ui->tributaryComboBox->currentText();
}

int GridCreatingConditionGridCombineSettingDialog::mainstreamCrossingBeginningIndex()
{
	return ui->mainstreamCrossingBeginningSpinBox->value();
}

int GridCreatingConditionGridCombineSettingDialog::mainstreamCrossingEndingIndex()
{
	return ui->mainstreamCrossingEndingSpinBox->value();
}

int GridCreatingConditionGridCombineSettingDialog::tributaryCrossingBeginningIndex()
{
	return ui->tributaryCrossingBeginningSpinBox->value();
}

int GridCreatingConditionGridCombineSettingDialog::tributaryCrossingEndingIndex()
{
	return ui->tributaryCrossingEndingSpinBox->value();
}

int GridCreatingConditionGridCombineSettingDialog::mainstreamFlowBeginningIndex()
{
	return ui->mainstreamFlowBeginningSpinBox->value();
}

int GridCreatingConditionGridCombineSettingDialog::mainstreamFlowEndingIndex()
{
	return ui->mainstreamFlowEndingSpinBox->value();
}

double GridCreatingConditionGridCombineSettingDialog::obstacleUpstreamX()
{
	return ui->upstreamXSpinBox->value();
}

double GridCreatingConditionGridCombineSettingDialog::obstacleUpstreamY()
{
	return ui->upstreamYSpinBox->value();
}

double GridCreatingConditionGridCombineSettingDialog::obstacleDownstreamX()
{
	return ui->downstreamXSpinBox->value();
}

double GridCreatingConditionGridCombineSettingDialog::obstacleDownstreamY()
{
	return ui->downstreamYSpinBox->value();
}

double GridCreatingConditionGridCombineSettingDialog::correctionValue()
{
	return ui->correctionSpinBox->value();
}

double GridCreatingConditionGridCombineSettingDialog::obstacleElevation()
{
	return ui->elevationSpinBox->value();
}

void GridCreatingConditionGridCombineSettingDialog::setupConnections()
{
	connect(ui->bindRadioButton, SIGNAL(toggled(bool)), ui->obstacleTab, SLOT(setDisabled(bool)));

	connect(ui->bindRadioButton, SIGNAL(toggled(bool)), this, SLOT(informConfluenceTypeChange(bool)));
	connect(ui->leftRadioButton, SIGNAL(toggled(bool)), this, SLOT(informConfluenceTypeChange(bool)));
	connect(ui->rightRadioButton, SIGNAL(toggled(bool)), this, SLOT(informConfluenceTypeChange(bool)));
	connect(ui->mainstreamComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(informGridChange(int)));
	connect(ui->tributaryComboBox, SIGNAL(currentIndexChanged(int)), SLOT(informGridChange(int)));
	connect(this, SIGNAL(gridInformationChanged()), this, SLOT(setupGridIndex()));

	connect(ui->mainstreamFlowBeginningSpinBox, SIGNAL(valueChanged(int)), this, SLOT(changeFlowEndingIndex(int)));
}

void GridCreatingConditionGridCombineSettingDialog::setupDisability()
{
	ui->obstacleTab->setDisabled(ui->bindRadioButton->isChecked());

	ui->mainstreamCrossingBeginningSpinBox->setDisabled(true);
	ui->mainstreamCrossingEndingSpinBox->setDisabled(true);
	ui->tributaryCrossingBeginningSpinBox->setDisabled(true);
	ui->tributaryCrossingEndingSpinBox->setDisabled(true);
	ui->mainstreamFlowEndingSpinBox->setDisabled(true);
}

void GridCreatingConditionGridCombineSettingDialog::setupComboBox(PreProcessorGridCreatingConditionDataItemInterface *item)
{
	PreProcessorGridTypeDataItemInterface* gt = dynamic_cast<PreProcessorGridTypeDataItemInterface*>(item->parent()->parent());
	PreProcessorGridAndGridCreatingConditionDataItemInterface* itemParent = dynamic_cast<PreProcessorGridAndGridCreatingConditionDataItemInterface*>(item->parent());

	QList<PreProcessorGridAndGridCreatingConditionDataItemInterface*>::const_iterator it;
	for (it = gt->conditions().begin(); it != gt->conditions().end(); ++it){
		if (*it == itemParent) continue;
		ui->mainstreamComboBox->addItem((*it)->caption());
		ui->tributaryComboBox->addItem((*it)->caption());
		m_gridMap.insert((*it)->caption(), (*it)->gridDataItem()->grid());
	}
}

void GridCreatingConditionGridCombineSettingDialog::informConfluenceTypeChange(bool b)
{
	if (b){
		emit gridInformationChanged();
	}
}

void GridCreatingConditionGridCombineSettingDialog::informGridChange(int /*i*/)
{
	emit gridInformationChanged();
}

void GridCreatingConditionGridCombineSettingDialog::setupGridIndex()
{
	Structured2DGrid* mg = dynamic_cast<Structured2DGrid*>(m_gridMap.value(ui->mainstreamComboBox->currentText()));
	Structured2DGrid* tg = dynamic_cast<Structured2DGrid*>(m_gridMap.value(ui->tributaryComboBox->currentText()));
	if (mg == 0 || tg == 0){
		ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
		return;
	} else {
		ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
	}
	int mi = mg->dimensionI();
	int mj = mg->dimensionJ();
	int ti = tg->dimensionI();
	int tj = tg->dimensionJ();
	offset = tj - 1;


	// Bind
	if (ui->bindRadioButton->isChecked()){
		if (mi != ti){
			ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
			return;
		} else {
			ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
		}

		ui->mainstreamCrossingBeginningSpinBox->setValue(0);
		ui->mainstreamCrossingEndingSpinBox->setValue(mj - 1);
		ui->tributaryCrossingBeginningSpinBox->setValue(mj);
		ui->tributaryCrossingEndingSpinBox->setValue(mj + tj - 1);
		ui->mainstreamFlowBeginningSpinBox->setMaximum(mi - 1);
		ui->mainstreamFlowEndingSpinBox->setValue(mi - 1);

		return;
	}

	// Left or Right
	if (tj > mi){
		ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
		return;
	} else {
		ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
	}
	ui->mainstreamFlowBeginningSpinBox->setValue(0);
	ui->mainstreamFlowBeginningSpinBox->setMaximum(mi - tj);
	ui->mainstreamFlowEndingSpinBox->setValue(tj - 1);

	// Left
	if (ui->leftRadioButton->isChecked()){
		ui->mainstreamCrossingBeginningSpinBox->setValue(0);
		ui->mainstreamCrossingEndingSpinBox->setValue(mj - 1);
		ui->tributaryCrossingBeginningSpinBox->setValue(mj - 1);
		ui->tributaryCrossingEndingSpinBox->setValue(mj + ti - 2);

		return;
	}

	// Right
	if (ui->rightRadioButton->isChecked()){
		ui->mainstreamCrossingBeginningSpinBox->setValue(ti - 1);
		ui->mainstreamCrossingEndingSpinBox->setValue(mj + ti - 2);
		ui->tributaryCrossingBeginningSpinBox->setValue(ti - 1);
		ui->tributaryCrossingEndingSpinBox->setValue(0);

		return;
	}
}

void GridCreatingConditionGridCombineSettingDialog::changeFlowEndingIndex(int index)
{
	if (ui->bindRadioButton->isChecked()) return;

	ui->mainstreamFlowEndingSpinBox->setValue(index + offset);
}
