#include "postparticlescalarpropertydialog.h"
#include "ui_postparticlescalarpropertydialog.h"
#include "postwindowgridtypedataiteminterface.h"

#include <guibase/scalarbardialog.h>
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

std::string PostParticleScalarPropertyDialog::target() const
{
	if (ui->arbitraryRadioButton->isChecked()) {
		return "";
	}
	return iRIC::toStr(ui->attributeComboBox->currentText());
}

void PostParticleScalarPropertyDialog::setTarget(const std::string& target)
{
	if (target == "") {
		ui->arbitraryRadioButton->setChecked(true);
		solutionChanged(0);
	} else {
		ui->byValueRadioButton->setChecked(true);
		ui->attributeComboBox->setCurrentText(target.c_str());
		solutionChanged(ui->attributeComboBox->currentIndex());
	}
}

const LookupTableContainer& PostParticleScalarPropertyDialog::lookupTable() const
{
	return m_lookupTable;
}

void PostParticleScalarPropertyDialog::setZoneData(PostZoneDataContainer* zoneData)
{
	vtkPointData* pd = zoneData->particleData()->GetPointData();
	int num = pd->GetNumberOfArrays();
	ui->attributeComboBox->clear();
	m_attributes.clear();
	ui->attributeComboBox->blockSignals(true);
	SolverDefinitionGridType* gtype = m_gridTypeDataItem->gridType();

	for (int i = 0; i < num; ++i) {
		vtkAbstractArray* tmparray = pd->GetArray(i);
		if (tmparray == nullptr) {continue;}
		std::string name = tmparray->GetName();
		if (tmparray->GetNumberOfComponents() > 1) {
			// vector attributes.
			continue;
		}
		ui->attributeComboBox->addItem(gtype->solutionCaption(name));
		m_attributes.append(name.c_str());
	}
	ui->attributeComboBox->blockSignals(false);
}

void PostParticleScalarPropertyDialog::setGridTypeDataItem(PostWindowGridTypeDataItemInterface* item)
{
	m_gridTypeDataItem = item;
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

void PostParticleScalarPropertyDialog::setScalarBarSetting(const ScalarBarSetting& setting)
{
	m_scalarBarSetting = setting;
}

void PostParticleScalarPropertyDialog::setTitleTextSetting(const vtkTextPropertySettingContainer& cont)
{
	m_titleTextSetting = cont;
}

void PostParticleScalarPropertyDialog::setLabelTextSetting(const vtkTextPropertySettingContainer& cont)
{
	m_labelTextSetting = cont;
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
	std::string att = m_attributes.at(index);
	LookupTableContainer* c = m_gridTypeDataItem->particleLookupTable(att);
	m_lookupTable = *c;
	ui->colormapWidget->setContainer(&m_lookupTable);
}

void PostParticleScalarPropertyDialog::showScalarBarDialog()
{
	ScalarBarDialog dialog(this);
	dialog.setTitle(m_scalarBarTitleMap[target()]);
	dialog.setSetting(m_scalarBarSetting);
	dialog.setTitleTextSetting(m_titleTextSetting);
	dialog.setLabelTextSetting(m_labelTextSetting);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	m_scalarBarTitleMap[target()] = dialog.title();
	m_scalarBarSetting = dialog.setting();
	m_titleTextSetting = dialog.titleTextSetting();
	m_labelTextSetting = dialog.labelTextSetting();
}
