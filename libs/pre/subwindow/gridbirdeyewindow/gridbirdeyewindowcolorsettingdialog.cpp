#include "ui_gridbirdeyewindowcolorsettingdialog.h"

#include "gridbirdeyewindowcolorsettingdialog.h"

#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>

GridBirdEyeWindowColorSettingDialog::GridBirdEyeWindowColorSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::GridBirdEyeWindowColorSettingDialog)
{
	ui->setupUi(this);
}

GridBirdEyeWindowColorSettingDialog::~GridBirdEyeWindowColorSettingDialog()
{
	delete ui;
}

void GridBirdEyeWindowColorSettingDialog::setGridType(SolverDefinitionGridType* type)
{
	ui->nodeAttComboBox->clear();
	QList<SolverDefinitionGridAttribute*> conds = type->gridRelatedConditions();
	for (int i = 0; i < conds.count(); ++i) {
		SolverDefinitionGridAttribute* c = conds.at(i);
		if (c->position() == SolverDefinitionGridAttribute::Node) {
			m_nodeConds.append(c);
			ui->nodeAttComboBox->addItem(c->caption(), c->name());
		} else if (c->position() == SolverDefinitionGridAttribute::CellCenter) {
			m_cellConds.append(c);
			ui->cellAttComboBox->addItem(c->caption(), c->name());
		}
	}
	if (m_nodeConds.count() == 0) {ui->nodeAttRadioButton->setDisabled(true);}
	if (m_cellConds.count() == 0) {ui->cellAttRadioButton->setDisabled(true);}
}

void GridBirdEyeWindowColorSettingDialog::setColorType(GridBirdEyeWindowDataModel::ColorType type)
{
	switch (type) {
	case GridBirdEyeWindowDataModel::ctNode:
		ui->nodeAttRadioButton->setChecked(true);
		ui->nodeAttRadioButton->setFocus();
		break;
	case GridBirdEyeWindowDataModel::ctCell:
		ui->cellAttRadioButton->setChecked(true);
		ui->cellAttRadioButton->setFocus();
		break;
	case GridBirdEyeWindowDataModel::ctCustom:
		ui->customColorRadioButton->setChecked(true);
		ui->customColorRadioButton->setFocus();
		break;
	}
}

void GridBirdEyeWindowColorSettingDialog::setAttributeName(const QString& name)
{
	if (ui->nodeAttRadioButton->isChecked()) {
		for (int i = 0; i < m_nodeConds.count(); ++i) {
			if (m_nodeConds.at(i)->name() == name) {
				ui->nodeAttComboBox->setCurrentIndex(i);
			}
		}
	} else if (ui->cellAttRadioButton->isChecked()) {
		for (int i = 0; i < m_cellConds.count(); ++i) {
			if (m_cellConds.at(i)->name() == name) {
				ui->cellAttComboBox->setCurrentIndex(i);
			}
		}
	}
}

void GridBirdEyeWindowColorSettingDialog::setCustomColor(const QColor& c)
{
	ui->customColorLabel->setColor(c);
}

void GridBirdEyeWindowColorSettingDialog::setAxesVisible(bool visible)
{
	ui->axesVisibleCheckBox->setChecked(visible);
}

void GridBirdEyeWindowColorSettingDialog::setAxisColor(const QColor& c)
{
	ui->axesColorWidget->setColor(c);
}

bool GridBirdEyeWindowColorSettingDialog::axesVisible()
{
	return ui->axesVisibleCheckBox->isChecked();
}

QColor GridBirdEyeWindowColorSettingDialog::axesColor()
{
	return ui->axesColorWidget->color();
}

GridBirdEyeWindowDataModel::ColorType GridBirdEyeWindowColorSettingDialog::colorType()
{
	if (ui->nodeAttRadioButton->isChecked()) {
		return GridBirdEyeWindowDataModel::ctNode;
	} else if (ui->cellAttRadioButton->isChecked()) {
		return GridBirdEyeWindowDataModel::ctCell;
	} else {
		return GridBirdEyeWindowDataModel::ctCustom;
	}
}

QString GridBirdEyeWindowColorSettingDialog::attributeName() const
{
	if (ui->nodeAttRadioButton->isChecked()) {
		return m_nodeConds.at(ui->nodeAttComboBox->currentIndex())->name();
	} else if (ui->cellAttRadioButton->isChecked()) {
		return m_cellConds.at(ui->cellAttComboBox->currentIndex())->name();
	}
	return "";
}

QColor GridBirdEyeWindowColorSettingDialog::customColor() const
{
	return ui->customColorLabel->color();
}
