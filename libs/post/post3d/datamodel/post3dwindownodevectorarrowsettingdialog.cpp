#include "post3dwindownodevectorarrowsettingdialog.h"
#include "ui_post3dwindownodevectorarrowsettingdialog.h"

#include <guibase/comboboxtool.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>

#include <vtkPointData.h>
#include <vtkStructuredGrid.h>

Post3dWindowNodeVectorArrowSettingDialog::Post3dWindowNodeVectorArrowSettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::Post3dWindowNodeVectorArrowSettingDialog),
	m_currentRow {-1}
{
	ui->setupUi(this);

	connect(ui->faceAddButton, SIGNAL(clicked()), this, SLOT(addFace()));
	connect(ui->faceRemoveButton, SIGNAL(clicked()), this, SLOT(removeFace()));
	connect(ui->faceListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(setCurrentFace(int)));
}

Post3dWindowNodeVectorArrowSettingDialog::~Post3dWindowNodeVectorArrowSettingDialog()
{
	delete ui;
}

void Post3dWindowNodeVectorArrowSettingDialog::setZoneData(PostZoneDataContainer* zoneData)
{
	vtkStructuredGrid* grid = dynamic_cast<vtkStructuredGrid*> (zoneData->data());
	grid->GetDimensions(m_gridDims);

	auto gType = zoneData->gridType();
	auto pd = zoneData->data()->GetPointData();

	m_targets = vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(pd);
	ComboBoxTool::setupItems(gType->solutionCaptions(m_targets), ui->physicalValueComboBox);

	ui->faceSettingWidget->setZoneData(zoneData);
}

ArrowSettingContainer Post3dWindowNodeVectorArrowSettingDialog::arrowSetting() const
{
	ArrowSettingContainer ret;

	ret.target = m_targets.at(ui->physicalValueComboBox->currentIndex()).c_str();
	if (ui->lengthAutoCheckBox->isChecked()) {
		ret.lengthMode = ArrowSettingContainer::LengthMode::Auto;
	} else {
		ret.lengthMode = ArrowSettingContainer::LengthMode::Custom;
	}
	ret.standardValue = ui->stdValueSpinBox->value();
	ret.legendLength = ui->legendLengthSpinBox->value();
	ret.minimumValue = ui->minValueSpinBox->value();

	return ret;
}

void Post3dWindowNodeVectorArrowSettingDialog::setArrowSetting(const ArrowSettingContainer& setting)
{
	auto target = iRIC::toStr(setting.target);
	auto it = std::find(m_targets.begin(), m_targets.end(), target);
	if (it != m_targets.end()) {
		auto index = it - m_targets.begin();
		ui->physicalValueComboBox->setCurrentIndex(index);
	}

	ui->lengthAutoCheckBox->setChecked(setting.lengthMode == ArrowSettingContainer::LengthMode::Auto);
	ui->stdValueSpinBox->setValue(setting.standardValue);
	ui->legendLengthSpinBox->setValue(setting.legendLength);
	ui->minValueSpinBox->setValue(setting.minimumValue);
}

std::vector<Post3dWindowNodeVectorArrowSettingDialog::FaceSetting> Post3dWindowNodeVectorArrowSettingDialog::faceSettings() const
{
	return m_faceSettings;
}

void Post3dWindowNodeVectorArrowSettingDialog::setFaceSettings(const std::vector<FaceSetting>& faceSettings)
{
	m_faceSettings = faceSettings;

	// update list
	auto listWidget = ui->faceListWidget;
	listWidget->clear();
	for (const auto& fs : m_faceSettings) {
		listWidget->addItem(fs.caption);
	}
	if (m_faceSettings.size() == 0) {
		ui->faceSettingWidget->setDisabled(true);
	} else {
		listWidget->setCurrentRow(0);
	}
}

void Post3dWindowNodeVectorArrowSettingDialog::accept()
{
	saveCurrentFace();
	QDialog::accept();
}

void Post3dWindowNodeVectorArrowSettingDialog::addFace()
{
	ui->faceSettingWidget->setEnabled(true);

	FaceSetting newSetting;

	if (m_faceSettings.size() == 0) {
		// create default
		auto& fs = newSetting.faceSetting;
		fs.direction = Post3dFaceSettingContainer::Direction::dirK;
		fs.iMin = 0;
		fs.iMax = m_gridDims[0] - 1;
		fs.jMin = 0;
		fs.jMax = m_gridDims[1] - 1;
		fs.kMin = 0;
		fs.kMax = 0;
	} else {
		// copy current
		saveCurrentFace();
		newSetting = m_faceSettings.at(ui->faceListWidget->currentRow());
	}
	newSetting.caption = tr("Face%1").arg(m_faceSettings.size() + 1);

	m_faceSettings.push_back(newSetting);
	updateFaceList();

	int row = m_faceSettings.size() - 1;
	ui->faceListWidget->setCurrentRow(row);
	ui->faceListWidget->scrollToItem(ui->faceListWidget->item(row));

	setCurrentFace(row);
}

void Post3dWindowNodeVectorArrowSettingDialog::removeFace()
{
	if (m_faceSettings.size() == 0) {return;}

	int row = ui->faceListWidget->currentRow();
	m_faceSettings.erase(m_faceSettings.begin() + row);
	m_currentRow = -1;

	updateFaceList();

	if (m_faceSettings.size() == 0) {
		FaceSetting dummy;
		ui->faceSettingWidget->setSettings(true, dummy.arrowSetting, dummy.faceSetting, dummy.samplingRateSetting);
		ui->faceSettingWidget->setDisabled(true);
		return;
	} else if (row >= m_faceSettings.size()) {
		row = m_faceSettings.size() - 1;
	}
	ui->faceListWidget->setCurrentRow(row);
}

void Post3dWindowNodeVectorArrowSettingDialog::setCurrentFace(int row)
{
	if (m_currentRow == row) {return;}

	saveCurrentFace();

	const auto& fs = m_faceSettings[row];
	ui->faceSettingWidget->setSettings(fs.enabled, fs.arrowSetting, fs.faceSetting, fs.samplingRateSetting);
	m_currentRow = row;
}

void Post3dWindowNodeVectorArrowSettingDialog::updateFaceList()
{
	auto w = ui->faceListWidget;
	w->blockSignals(true);

	w->clear();
	for (auto fs : m_faceSettings) {
		w->addItem(fs.caption);
	}

	w->blockSignals(false);
}

void Post3dWindowNodeVectorArrowSettingDialog::saveCurrentFace()
{
	if (m_currentRow == -1) {return;}

	auto& fs = m_faceSettings[m_currentRow];
	auto w = ui->faceSettingWidget;

	fs.enabled = w->enabled();
	fs.arrowSetting = w->arrowSetting();
	fs.faceSetting = w->faceSetting();
	fs.samplingRateSetting = w->samplingRateSetting();
}
