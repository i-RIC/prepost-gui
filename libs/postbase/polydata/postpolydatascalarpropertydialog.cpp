#include "postpolydatascalarpropertydialog.h"
#include "ui_postpolydatascalarpropertydialog.h"
#include "postwindowgridtypedataiteminterface.h"

#include <guibase/comboboxtool.h>
#include <guibase/scalarbardialog.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/stringtool.h>

#include <vtkCellData.h>
#include <vtkPolyData.h>

PostPolyDataScalarPropertyDialog::PostPolyDataScalarPropertyDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::PostPolyDataScalarPropertyDialog)
{
	ui->setupUi(this);
	ui->colormapWidget->hideDivisionNumber();
	ui->colormapWidget->hideFillSettings();

	connect(ui->attributeComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(solutionChanged(int)));
	connect(ui->colorbarSettingButton, SIGNAL(clicked()), this, SLOT(showScalarBarDialog()));
}

PostPolyDataScalarPropertyDialog::~PostPolyDataScalarPropertyDialog()
{
	delete ui;
}

void PostPolyDataScalarPropertyDialog::setGridTypeDataItem(PostWindowGridTypeDataItemInterface* item)
{
	m_gridTypeDataItem = item;
}

void PostPolyDataScalarPropertyDialog::setPolyData(vtkPolyData* polyData)
{
	vtkCellData* cd = polyData->GetCellData();
	SolverDefinitionGridType* gtype = m_gridTypeDataItem->gridType();

	m_targets = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cd);
	ComboBoxTool::setupItems(gtype->solutionCaptions(m_targets), ui->attributeComboBox);
}

ScalarSettingContainer PostPolyDataScalarPropertyDialog::setting() const
{
	ScalarSettingContainer ret = m_setting;

	if (ui->arbitraryRadioButton->isChecked()) {
		ret.target = "";
	} else {
		ret.target = ui->attributeComboBox->currentText();
	}

	return ret;
}

void PostPolyDataScalarPropertyDialog::setSetting(const ScalarSettingContainer setting)
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

const LookupTableContainer& PostPolyDataScalarPropertyDialog::lookupTable() const
{
	return m_lookupTable;
}

QString PostPolyDataScalarPropertyDialog::scalarBarTitle() const
{
	std::string targ = target();
	if (targ == "") {return "";}
	return m_scalarBarTitleMap.at(targ);
}

void PostPolyDataScalarPropertyDialog::setScalarBarTitleMap(const std::map<std::string, QString>& titlemap)
{
	m_scalarBarTitleMap = titlemap;
}

QColor PostPolyDataScalarPropertyDialog::customColor() const
{
	return ui->colorEditWidget->color();
}

void PostPolyDataScalarPropertyDialog::setCustomColor(const QColor& c)
{
	ui->colorEditWidget->setColor(c);
}

int PostPolyDataScalarPropertyDialog::lineWidgh() const
{
	return ui->widthSpinBox->value();
}

void PostPolyDataScalarPropertyDialog::setLineWidth(int width)
{
	ui->widthSpinBox->setValue(width);
}

void PostPolyDataScalarPropertyDialog::accept()
{
	ui->colormapWidget->save();
	QDialog::accept();
}

void PostPolyDataScalarPropertyDialog::solutionChanged(int index)
{
	std::string att = m_targets.at(index);
	LookupTableContainer* c = m_gridTypeDataItem->polyDataLookupTable(att);
	m_lookupTable = *c;
	ui->colormapWidget->setContainer(&m_lookupTable);
}

void PostPolyDataScalarPropertyDialog::showScalarBarDialog()
{
	ScalarBarDialog dialog(this);
	dialog.setSetting(m_setting.scalarBarSetting);
	dialog.setTitle(m_scalarBarTitleMap.at(target()));

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	m_setting.scalarBarSetting = dialog.setting();
	m_scalarBarTitleMap.at(target()) = dialog.title();
}

std::string PostPolyDataScalarPropertyDialog::target() const
{
	if (ui->arbitraryRadioButton->isChecked()) {
		return "";
	}
	return iRIC::toStr(ui->attributeComboBox->currentText());
}
