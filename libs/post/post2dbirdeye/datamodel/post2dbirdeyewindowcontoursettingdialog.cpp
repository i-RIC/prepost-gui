#include "ui_post2dbirdeyewindowcontoursettingdialog.h"

#include "post2dbirdeyewindowcontoursettingdialog.h"

#include <guibase/comboboxtool.h>
#include <guibase/scalarbardialog.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <postbase/postwindowgridtypedataiteminterface.h>
#include <post/post2d/post2dgridregionselectdialog.h>

#include <QMessageBox>

#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkStructuredGrid.h>

Post2dBirdEyeWindowContourSettingDialog::Post2dBirdEyeWindowContourSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Post2dBirdEyeWindowContourSettingDialog),
	m_activeAvailable {true},
	m_unstructured {false},
	m_position {iRICLib::H5CgnsZone::SolutionPosition::Null}
{
	ui->setupUi(this);
	ui->contourWidget->hidePointsRadioButton();

	m_colorButtonGroup.addButton(ui->specificRadioButton);
	m_colorButtonGroup.addButton(ui->scalarRadioButton);

	connect(ui->physicalValueComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(targetChanged(int)));
	connect(ui->regionSettingButton, SIGNAL(clicked()), this, SLOT(showRegionDialog()));
	connect(ui->colorbarSettingButton, SIGNAL(clicked()), this, SLOT(showScalarBarDialog()));
	connect(ui->contourWidget, SIGNAL(contourChanged(ContourSettingWidget::Contour)), ui->colormapWidget, SLOT(setContourSetting(ContourSettingWidget::Contour)));
}

Post2dBirdEyeWindowContourSettingDialog::~Post2dBirdEyeWindowContourSettingDialog()
{
	delete ui;
}

void Post2dBirdEyeWindowContourSettingDialog::setZoneData(PostZoneDataContainer* zoneData, iRICLib::H5CgnsZone::SolutionPosition position)
{
	SolverDefinitionGridType* gtype = m_gridTypeDataItem->gridType();

	if (position == iRICLib::H5CgnsZone::SolutionPosition::Node || position == iRICLib::H5CgnsZone::SolutionPosition::IFace || position == iRICLib::H5CgnsZone::SolutionPosition::JFace) {
		vtkPointData* pd = zoneData->data(position)->GetPointData();
		m_solutions.clear();
		for (const auto& name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(pd)) {
			if (PostZoneDataContainer::hasInputDataPrefix(name)) {continue;}
			m_solutions.push_back(name);
		}
	} else if (position == iRICLib::H5CgnsZone::SolutionPosition::Cell) {
		vtkCellData* cd = zoneData->data()->GetCellData();
		m_solutions.clear();
		for (const auto& name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cd)) {
			if (PostZoneDataContainer::hasInputDataPrefix(name)) {continue;}
			m_solutions.push_back(name);
		}
		ui->contourWidget->hideRadioButton(ContourSettingWidget::ContourFigure);
	}

	m_position = position;

	ComboBoxTool::setupItems(gtype->solutionCaptions(m_solutions), ui->scalarForElevationComboBox);
	ComboBoxTool::setupItems(gtype->solutionCaptions(m_solutions), ui->physicalValueComboBox);

	vtkStructuredGrid* stG = dynamic_cast<vtkStructuredGrid*> (zoneData->data());
	m_unstructured = (stG == nullptr);

	if (stG != nullptr) {
		stG->GetDimensions(m_gridDims);
	}
}

void Post2dBirdEyeWindowContourSettingDialog::setGridTypeDataItem(PostWindowGridTypeDataItemInterface* item)
{
	m_gridTypeDataItem = item;
}

void Post2dBirdEyeWindowContourSettingDialog::disableActive()
{
	m_activeAvailable = false;
}

void Post2dBirdEyeWindowContourSettingDialog::disablePhysicalValueComboBox()
{
	ui->physicalValueComboBox->setDisabled(true);
}

void Post2dBirdEyeWindowContourSettingDialog::setSetting(const Post2dBirdEyeWindowContourSetting& s)
{
	m_setting = s;

	auto it = std::find(m_solutions.begin(), m_solutions.end(), std::string(s.elevationTarget));
	if (it == m_solutions.end()) {it = m_solutions.begin();}
	ui->scalarForElevationComboBox->setCurrentIndex(it - m_solutions.begin());

	it = std::find(m_solutions.begin(), m_solutions.end(), std::string(s.target));
	if (it == m_solutions.end()) {it = m_solutions.begin();}
	ui->physicalValueComboBox->setCurrentIndex(it - m_solutions.begin());
	targetChanged(it - m_solutions.begin());

	if (s.colorMode == Post2dBirdEyeWindowContourSetting::ColorMode::Custom) {
		ui->specificRadioButton->setChecked(true);
	} else {
		ui->scalarRadioButton->setChecked(true);
	}
	ui->colorEditWidget->setColor(s.customColor);
	ui->contourWidget->setContour(s.contour);
	ui->colormapWidget->setDivisionNumber(s.numberOfDivisions);
	ui->colormapWidget->setFillUpper(s.fillUpper);
	ui->colormapWidget->setFillLower(s.fillLower);
	ui->transparencyWidget->setOpacityPercent(s.opacity);
}

void Post2dBirdEyeWindowContourSettingDialog::setColorBarTitleMap(const QMap<std::string, QString>& titlemap)
{
	m_colorBarTitleMap = titlemap;
}

Post2dBirdEyeWindowContourSetting Post2dBirdEyeWindowContourSettingDialog::setting() const
{
	Post2dBirdEyeWindowContourSetting ret = m_setting;

	ret.elevationTarget = m_solutions.at(ui->scalarForElevationComboBox->currentIndex()).c_str();

	if (ui->specificRadioButton->isChecked()) {
		ret.colorMode = Post2dBirdEyeWindowContourSetting::ColorMode::Custom;
	} else {
		ret.colorMode = Post2dBirdEyeWindowContourSetting::ColorMode::ByScalar;
	}
	ret.customColor = ui->colorEditWidget->color();
	ret.target = target().c_str();
	ret.contour = ui->contourWidget->contour();
	ret.numberOfDivisions = ui->colormapWidget->divisionNumber();
	ret.fillUpper = ui->colormapWidget->fillUpper();
	ret.fillLower = ui->colormapWidget->fillLower();
	ret.opacity = ui->transparencyWidget->opacityPercent();

	return ret;
}

QString Post2dBirdEyeWindowContourSettingDialog::scalarBarTitle()
{
	return m_colorBarTitleMap[target()];
}

std::string Post2dBirdEyeWindowContourSettingDialog::target() const
{
	return m_solutions.at(ui->physicalValueComboBox->currentIndex());
}

const LookupTableContainer& Post2dBirdEyeWindowContourSettingDialog::lookupTable()
{
	return m_lookupTable;
}

void Post2dBirdEyeWindowContourSettingDialog::targetChanged(int index)
{
	std::string sol = m_solutions.at(index);
	LookupTableContainer* c;
	if (m_position == iRICLib::H5CgnsZone::SolutionPosition::Node) {
		c = m_gridTypeDataItem->nodeLookupTable(sol);
	}
	else if (m_position == iRICLib::H5CgnsZone::SolutionPosition::Cell) {
		c = m_gridTypeDataItem->cellLookupTable(sol);
	}
	else if (m_position == iRICLib::H5CgnsZone::SolutionPosition::IFace) {
		c = m_gridTypeDataItem->nodeLookupTable(sol);
	}
	else if (m_position == iRICLib::H5CgnsZone::SolutionPosition::JFace) {
		c = m_gridTypeDataItem->nodeLookupTable(sol);
	}
	else {
		Q_ASSERT(false);
	}

	m_lookupTable = *c;
	ui->colormapWidget->setContainer(&m_lookupTable);
}

void Post2dBirdEyeWindowContourSettingDialog::accept()
{
	if ((! ui->colormapWidget->autoRange()) &&
			(ui->colormapWidget->upperLimit() < ui->colormapWidget->lowerLimit())) {
		QMessageBox::critical(this, tr("Error"), tr("For value range, max value is smaller than min value."));
		return;
	}

	ui->colormapWidget->save();
	QDialog::accept();
}

void Post2dBirdEyeWindowContourSettingDialog::showRegionDialog()
{
	Post2dGridRegionSelectDialog dialog(this);
	if (! m_activeAvailable) {
		dialog.disableActive();
	}
	if (m_unstructured) {
		dialog.hideCustom();
	}
	else {
		dialog.setGridSize(m_gridDims[0], m_gridDims[1]);
	}
	dialog.setRegionMode(m_setting.regionMode);
	dialog.setRegion(m_setting.range);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}
	m_setting.regionMode = dialog.regionMode();
	m_setting.range = dialog.region();
}

void Post2dBirdEyeWindowContourSettingDialog::showScalarBarDialog()
{
	ScalarBarDialog dialog(this);
	dialog.setSetting(m_setting.scalarBarSetting);
	dialog.setTitle(m_colorBarTitleMap[target()]);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	m_setting.scalarBarSetting = dialog.setting();
	m_colorBarTitleMap[target()] = dialog.title();
}
