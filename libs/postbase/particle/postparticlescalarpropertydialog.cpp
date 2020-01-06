#include "postparticlescalarpropertydialog.h"
#include "ui_postparticlescalarpropertydialog.h"
#include "postwindowgridtypedataiteminterface.h"

#include <guibase/comboboxtool.h>
#include <guibase/scalarbardialog.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/stringtool.h>

#include <vtkPolyData.h>
#include <vtkPointData.h>

PostParticleScalarPropertyDialog::PostParticleScalarPropertyDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PostParticleScalarPropertyDialog)
{
	ui->setupUi(this);
	ui->colormapWidget->hideDivisionNumber();
	ui->colormapWidget->hideFillSettings();

	connect(ui->attributeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(solutionChanged(int)));
	connect(ui->colorbarSettingButton, SIGNAL(clicked()), this, SLOT(showScalarBarDialog()));
}

PostParticleScalarPropertyDialog::~PostParticleScalarPropertyDialog()
{
	delete ui;
}

void PostParticleScalarPropertyDialog::setGridTypeDataItem(PostWindowGridTypeDataItemInterface* item)
{
	m_gridTypeDataItem = item;
}

void PostParticleScalarPropertyDialog::setData(vtkPolyData* data)
{
	auto pd = data->GetPointData();
	SolverDefinitionGridType* gtype = m_gridTypeDataItem->gridType();

	m_targets = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(pd);
	ComboBoxTool::setupItems(gtype->solutionCaptions(m_targets), ui->attributeComboBox);
}

ScalarSettingContainer PostParticleScalarPropertyDialog::setting() const
{
	ScalarSettingContainer ret = m_setting;

	if (ui->arbitraryRadioButton->isChecked()) {
		ret.target = "";
	} else {
		ret.target = ui->attributeComboBox->currentText();
	}

	return ret;
}

void PostParticleScalarPropertyDialog::setSetting(const ScalarSettingContainer setting)
{
	m_setting = setting;

	if (setting.target == "") {
		ui->arbitraryRadioButton->setChecked(true);
		solutionChanged(0);
	} else {
		ui->byValueRadioButton->setChecked(true);
		ui->attributeComboBox->setCurrentText(setting.target);
		solutionChanged(ui->attributeComboBox->currentIndex());
	}
}

const LookupTableContainer& PostParticleScalarPropertyDialog::lookupTable() const
{
	return m_lookupTable;
}

QString PostParticleScalarPropertyDialog::scalarBarTitle() const
{
	std::string targ = target();
	if (targ == "") {return "";}
	return m_scalarBarTitleMap[targ];
}

void PostParticleScalarPropertyDialog::setScalarBarTitleMap(const QMap<std::string, QString>& titlemap)
{
	m_scalarBarTitleMap = titlemap;
}

int PostParticleScalarPropertyDialog::particleSize() const
{
	return ui->sizeSpinBox->value();
}

void PostParticleScalarPropertyDialog::setParticleSize(int size)
{
	ui->sizeSpinBox->setValue(size);
}

QColor PostParticleScalarPropertyDialog::customColor() const
{
	return ui->colorEditWidget->color();
}

void PostParticleScalarPropertyDialog::setCustomColor(const QColor& c)
{
	ui->colorEditWidget->setColor(c);
}

void PostParticleScalarPropertyDialog::accept()
{
	ui->colormapWidget->save();
	QDialog::accept();
}

void PostParticleScalarPropertyDialog::solutionChanged(int index)
{
	std::string att = m_targets.at(index);
	LookupTableContainer* c = m_gridTypeDataItem->particleLookupTable(att);
	m_lookupTable = *c;
	ui->colormapWidget->setContainer(&m_lookupTable);
}

void PostParticleScalarPropertyDialog::showScalarBarDialog()
{
	ScalarBarDialog dialog(this);
	dialog.setSetting(m_setting.scalarBarSetting);
	dialog.setTitle(m_scalarBarTitleMap[target()]);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	m_setting.scalarBarSetting = dialog.setting();
	m_scalarBarTitleMap[target()] = dialog.title();
}

std::string PostParticleScalarPropertyDialog::target() const
{
	if (ui->arbitraryRadioButton->isChecked()) {
		return "";
	}
	return iRIC::toStr(ui->attributeComboBox->currentText());
}
