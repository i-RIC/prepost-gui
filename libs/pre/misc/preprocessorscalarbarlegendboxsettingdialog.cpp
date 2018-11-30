#include "ui_preprocessorscalarbarlegendboxsettingdialog.h"

#include "../datamodel/preprocessorgeodatagroupdataitem.h"
#include "../datamodel/preprocessorgeodatatopdataitem.h"
#include "preprocessorscalarbarlegendboxsettingdialog.h"

#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
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

void PreProcessorScalarBarLegendBoxSettingDialog::setupComboBox(PreProcessorGeoDataTopDataItemInterface* item)
{
	QList<PreProcessorGeoDataGroupDataItemInterface*> groups = item->groupDataItems();
	for (auto group : groups) {
		auto condition = group->condition();
		if (condition->isReferenceInformation()) {continue;}

		QString text = condition->caption();
		int position = static_cast<int>(condition->position());
		ui->scalarComboBox->addItem(text, QVariant(position));
		m_itemMap.insert(text, group);
	}
}

void PreProcessorScalarBarLegendBoxSettingDialog::editScalarBar()
{
	PreProcessorGeoDataGroupDataItemInterface* group = m_itemMap.value(ui->scalarComboBox->currentText());
	PreProcessorGeoDataGroupDataItem* group2 = dynamic_cast<PreProcessorGeoDataGroupDataItem*>(group);
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
	PreProcessorGeoDataGroupDataItem* group = dynamic_cast<PreProcessorGeoDataGroupDataItem*>(m_itemMap.value(ui->scalarComboBox->currentText()));
	return group->condition();
}

const vtkTextPropertySettingContainer& PreProcessorScalarBarLegendBoxSettingDialog::titleTextSetting() const
{
	return m_titleTextSetting;
}

const vtkTextPropertySettingContainer& PreProcessorScalarBarLegendBoxSettingDialog::labelTextSetting() const
{
	return m_labelTextSetting;
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
