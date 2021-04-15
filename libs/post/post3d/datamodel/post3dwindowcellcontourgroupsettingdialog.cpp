#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowcellcontourgroupsettingdialog.h"
#include "ui_post3dwindowcellcontourgroupsettingdialog.h"

#include <guibase/comboboxtool.h>
#include <guibase/scalarbardialog.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/stringtool.h>

#include <vtkCellData.h>
#include <vtkStructuredGrid.h>

Post3dWindowCellContourGroupSettingDialog::Post3dWindowCellContourGroupSettingDialog(QWidget *parent) :
	QDialog(parent),
	m_currentRow {-1},
	ui(new Ui::Post3dWindowCellContourGroupSettingDialog)
{
	ui->setupUi(this);
	ui->lookupTableWidget->hideDivisionNumber();
	ui->lookupTableWidget->setContainer(&m_lookupTable);

	connect(ui->valueComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(solutionChanged(int)));
	connect(ui->rangeAddButton, SIGNAL(clicked()), this, SLOT(addRange()));
	connect(ui->rangeRemoveButton, SIGNAL(clicked()), this, SLOT(removeRange()));
	connect(ui->rangeListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(setCurrentRange(int)));
	connect(ui->colorbarSettingButton, SIGNAL(clicked()), this, SLOT(showColorBarDialog()));
}

Post3dWindowCellContourGroupSettingDialog::~Post3dWindowCellContourGroupSettingDialog()
{
	delete ui;
}

void Post3dWindowCellContourGroupSettingDialog::setGridTypeDataItem(Post3dWindowGridTypeDataItem* item)
{
	m_gridTypeDataItem = item;
}

void Post3dWindowCellContourGroupSettingDialog::setZoneData(PostZoneDataContainer* zoneData)
{
	vtkStructuredGrid* grid = dynamic_cast<vtkStructuredGrid*> (zoneData->data());
	grid->GetDimensions(m_gridDims);

	auto gType = zoneData->gridType();
	auto cd = zoneData->data()->GetCellData();

	m_targets = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cd);
	ComboBoxTool::setupItems(gType->solutionCaptions(m_targets), ui->valueComboBox);

	ui->rangeSettingWidget->setZoneData(zoneData);
}

void Post3dWindowCellContourGroupSettingDialog::setColorBarTitleMap(const QMap<std::string, QString>& titleMap)
{
	m_colorBarTitleMap = titleMap;
}

ScalarSettingContainer Post3dWindowCellContourGroupSettingDialog::scalarSetting() const
{
	ScalarSettingContainer ret = m_scalarSetting;

	ret.target = ui->valueComboBox->currentText();
	ret.fillUpper = ui->lookupTableWidget->fillUpper();
	ret.fillLower = ui->lookupTableWidget->fillLower();

	return ret;
}

void Post3dWindowCellContourGroupSettingDialog::setScalarSetting(const ScalarSettingContainer& setting)
{
	m_scalarSetting = setting;

	ui->valueComboBox->setCurrentText(setting.target);
	ui->lookupTableWidget->setFillUpper(setting.fillUpper);
	ui->lookupTableWidget->setFillLower(setting.fillLower);
}

LookupTableContainer Post3dWindowCellContourGroupSettingDialog::lookupTable() const
{
	return m_lookupTable;
}

void Post3dWindowCellContourGroupSettingDialog::setLookupTable(const LookupTableContainer& c)
{
	m_lookupTable = c;
	ui->lookupTableWidget->setContainer(&m_lookupTable);
}

std::vector<Post3dCellRangeSettingContainer> Post3dWindowCellContourGroupSettingDialog::rangeSettings()
{
	return m_rangeSettings;
}

void Post3dWindowCellContourGroupSettingDialog::setRangeSettings(const std::vector<Post3dCellRangeSettingContainer> rangeSettings)
{
	m_rangeSettings = rangeSettings;
	updateRangeList();

	if (m_rangeSettings.size() == 0) {
		ui->rangeSettingWidget->setDisabled(true);
	} else {
		ui->rangeListWidget->setCurrentRow(0);
	}
}

QString Post3dWindowCellContourGroupSettingDialog::scalarBarTitle()
{
	return m_colorBarTitleMap[target()];
}

void Post3dWindowCellContourGroupSettingDialog::accept()
{
	ui->lookupTableWidget->save();
	saveCurrentRange();

	QDialog::accept();
}

void Post3dWindowCellContourGroupSettingDialog::addRange()
{
	ui->rangeSettingWidget->setEnabled(true);

	Post3dCellRangeSettingContainer newSetting;

	if (m_rangeSettings.size() == 0) {
		// create default
		newSetting.enabled = true;
		newSetting.iMin = 0;
		newSetting.iMax = m_gridDims[0] - 2;
		newSetting.jMin = 0;
		newSetting.jMax = m_gridDims[1] - 2;
		newSetting.kMin = 0;
		newSetting.kMax = m_gridDims[2] - 2;
	} else {
		// copy current
		saveCurrentRange();
		newSetting = m_rangeSettings.at(ui->rangeListWidget->currentRow());
	}

	m_rangeSettings.push_back(newSetting);
	updateRangeList();

	int row = m_rangeSettings.size() - 1;
	ui->rangeListWidget->setCurrentRow(row);
	ui->rangeListWidget->scrollToItem(ui->rangeListWidget->item(row));

	setCurrentRange(row);
}

void Post3dWindowCellContourGroupSettingDialog::removeRange()
{
	if (m_rangeSettings.size() == 0) {return;}

	int row = ui->rangeListWidget->currentRow();
	m_rangeSettings.erase(m_rangeSettings.begin() + row);
	m_currentRow = -1;

	updateRangeList();

	if (m_rangeSettings.size() == 0) {
		Post3dCellRangeSettingContainer dummy;
		ui->rangeSettingWidget->setSetting(dummy);
		ui->rangeSettingWidget->setDisabled(true);
		return;
	} else if (row >= m_rangeSettings.size()) {
		row = m_rangeSettings.size() - 1;
	}
	ui->rangeListWidget->setCurrentRow(row);
}

void Post3dWindowCellContourGroupSettingDialog::setCurrentRange(int row)
{
	if (m_currentRow == row) {return;}

	saveCurrentRange();

	const auto& rs = m_rangeSettings[row];
	ui->rangeSettingWidget->setSetting(rs);
	m_currentRow = row;
}

void Post3dWindowCellContourGroupSettingDialog::solutionChanged(int index)
{
	auto target = m_targets.at(index);
	auto lut = m_gridTypeDataItem->cellLookupTable(target);
	m_lookupTable = *lut;
	ui->lookupTableWidget->setContainer(&m_lookupTable);
}

void Post3dWindowCellContourGroupSettingDialog::showColorBarDialog()
{
	ScalarBarDialog dialog(this);

	dialog.setSetting(m_scalarSetting.scalarBarSetting);
	dialog.setTitle(m_colorBarTitleMap[target()]);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	m_scalarSetting.scalarBarSetting = dialog.setting();
	m_colorBarTitleMap[target()] = dialog.title();
}

void Post3dWindowCellContourGroupSettingDialog::updateRangeList()
{
	auto w = ui->rangeListWidget;
	w->blockSignals(true);

	w->clear();
	int idx = 1;
	for (auto rs : m_rangeSettings) {
		w->addItem(tr("Range%1").arg(idx));
		++idx;
	}
	w->blockSignals(false);
}

void Post3dWindowCellContourGroupSettingDialog::saveCurrentRange()
{
	if (m_currentRow == -1) {return;}

	auto& rs = m_rangeSettings[m_currentRow];
	auto w = ui->rangeSettingWidget;

	rs = w->setting();
}

std::string Post3dWindowCellContourGroupSettingDialog::target() const
{
	return iRIC::toStr(ui->valueComboBox->currentText());
}
