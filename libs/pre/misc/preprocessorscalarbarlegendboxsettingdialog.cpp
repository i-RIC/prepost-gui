#include "ui_preprocessorscalarbarlegendboxsettingdialog.h"

#include "../datamodel/preprocessorrawdatagroupdataitem.h"
#include "../datamodel/preprocessorrawdatatopdataitem.h"
#include "preprocessorscalarbarlegendboxsettingdialog.h"

#include <guicore/pre/base/preprocessorrawdatagroupdataiteminterface.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>

PreProcessorScalarBarLegendBoxSettingDialog::PreProcessorScalarBarLegendBoxSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::PreProcessorScalarBarLegendBoxSettingDialog)
{
	ui->setupUi(this);

	connect(ui->displayCheckBox, SIGNAL(toggled(bool)), this, SLOT(setDisplay(bool)));
	connect(ui->editButton, SIGNAL(clicked()), this, SLOT(editScalarBar()));
}

PreProcessorScalarBarLegendBoxSettingDialog::~PreProcessorScalarBarLegendBoxSettingDialog()
{
	delete ui;
}

void PreProcessorScalarBarLegendBoxSettingDialog::setDisplay(bool disp)
{
	ui->scalarGroupBox->setEnabled(disp);
}

void PreProcessorScalarBarLegendBoxSettingDialog::setupComboBox(PreProcessorRawDataTopDataItemInterface* item)
{
	QList<PreProcessorRawDataGroupDataItemInterface*> groups = item->groupDataItems();
	for (auto it = groups.begin(); it != groups.end(); ++it) {
		PreProcessorRawDataGroupDataItemInterface* group = *it;
		QString text = group->condition()->caption();
		int position = static_cast<int>(group->condition()->position());
		ui->scalarComboBox->addItem(text, QVariant(position));
		m_itemMap.insert(text, group);
	}
}

void PreProcessorScalarBarLegendBoxSettingDialog::editScalarBar()
{
	PreProcessorRawDataGroupDataItemInterface* group = m_itemMap.value(ui->scalarComboBox->currentText());
	PreProcessorRawDataGroupDataItem* group2 = dynamic_cast<PreProcessorRawDataGroupDataItem*>(group);
	group2->editScalarBarLegendBox(this);
}

void PreProcessorScalarBarLegendBoxSettingDialog::setCondition(SolverDefinitionGridAttribute* condition)
{
	if (condition == nullptr) {return;}
	int idx = ui->scalarComboBox->findText(condition->caption());
	ui->scalarComboBox->setCurrentIndex(idx);
}

void PreProcessorScalarBarLegendBoxSettingDialog::setTitleTextSetting(const vtkTextPropertySettingContainer& cont)
{
	m_titleTextSetting = cont;
}

void PreProcessorScalarBarLegendBoxSettingDialog::setLabelTextSetting(const vtkTextPropertySettingContainer& cont)
{
	m_labelTextSetting = cont;
}

SolverDefinitionGridAttribute* PreProcessorScalarBarLegendBoxSettingDialog::condition()
{
	PreProcessorRawDataGroupDataItem* group = dynamic_cast<PreProcessorRawDataGroupDataItem*>(m_itemMap.value(ui->scalarComboBox->currentText()));
	return group->condition();
}

void PreProcessorScalarBarLegendBoxSettingDialog::setActor2DVisibility(bool vis)
{
	if (vis) {
		ui->displayCheckBox->setCheckState(Qt::Checked);
	} else {
		ui->displayCheckBox->setCheckState(Qt::Unchecked);
	}
}

bool PreProcessorScalarBarLegendBoxSettingDialog::actor2DVisibility()
{
	return ui->displayCheckBox->isChecked();
}
