#include "ui_post2dwindowcontoursettingdialog.h"

#include "../post2dgridregionselectdialog.h"
#include "post2dwindowcontoursettingdialog.h"

#include <guibase/comboboxtool.h>
#include <guibase/scalarbardialog.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <postbase/postwindowgridtypedataiteminterface.h>

#include <QMessageBox>

#include <vtkCellData.h>
#include <vtkPointData.h>
#include <vtkStructuredGrid.h>

Post2dWindowContourSettingDialog::Post2dWindowContourSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Post2dWindowContourSettingDialog)
{
	m_activeAvailable = true;
	m_unstructured = false;
	m_location = GridLocationNull;

	ui->setupUi(this);
	ui->contourWidget->hidePointsRadioButton();

	connect(ui->physicalValueComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(targetChanged(int)));
	connect(ui->regionSettingButton, SIGNAL(clicked()), this, SLOT(showRegionDialog()));
	connect(ui->colorbarSettingButton, SIGNAL(clicked()), this, SLOT(showScalarBarDialog()));
	connect(ui->contourWidget, SIGNAL(contourChanged(ContourSettingWidget::Contour)), ui->colormapWidget, SLOT(setContourSetting(ContourSettingWidget::Contour)));
}

Post2dWindowContourSettingDialog::~Post2dWindowContourSettingDialog()
{
	delete ui;
}

void Post2dWindowContourSettingDialog::setZoneData(PostZoneDataContainer* zoneData, GridLocation_t location)
{
	SolverDefinitionGridType* gtype = m_gridTypeDataItem->gridType();

	if (location == Vertex) {
		vtkPointData* pd = zoneData->data()->GetPointData();
		m_solutions.clear();
		for (const auto& name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(pd)) {
			if (PostZoneDataContainer::hasInputDataPrefix(name)) {continue;}
			m_solutions.push_back(name);
		}
	} else if (location == CellCenter) {
		vtkCellData* cd = zoneData->data()->GetCellData();
		m_solutions.clear();
		for (const auto& name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cd)) {
			if (PostZoneDataContainer::hasInputDataPrefix(name)) {continue;}
			m_solutions.push_back(name);
		}
		ui->contourWidget->hideRadioButton(ContourSettingWidget::ContourFigure);
	}
	m_location = location;

	ComboBoxTool::setupItems(gtype->solutionCaptions(m_solutions), ui->physicalValueComboBox);

	vtkStructuredGrid* stG = dynamic_cast<vtkStructuredGrid*> (zoneData->data());
	m_unstructured = (stG == nullptr);

	if (stG != nullptr) {
		stG->GetDimensions(m_gridDims);
	}
}

void Post2dWindowContourSettingDialog::setGridTypeDataItem(PostWindowGridTypeDataItemInterface* item)
{
	m_gridTypeDataItem = item;
}

void Post2dWindowContourSettingDialog::disableActive()
{
	m_activeAvailable = false;
}

void Post2dWindowContourSettingDialog::disablePhysicalValueComboBox()
{
	ui->physicalValueComboBox->setDisabled(true);
}

void Post2dWindowContourSettingDialog::hideOpacity()
{
	ui->transparencyWidget->hide();
	QSize size = minimumSize();
	resize(size);
}

void Post2dWindowContourSettingDialog::setSetting(const Post2dWindowContourSetting& s)
{
	m_setting = s;

	auto it = std::find(m_solutions.begin(), m_solutions.end(), std::string(s.target));
	if (it == m_solutions.end()) {it = m_solutions.begin();}
	ui->physicalValueComboBox->setCurrentIndex(it - m_solutions.begin());
	targetChanged(it - m_solutions.begin());

	ui->contourWidget->setContour(s.contour);
	ui->colormapWidget->setDivisionNumber(s.numberOfDivisions);
	ui->colormapWidget->setFillUpper(s.fillUpper);
	ui->colormapWidget->setFillLower(s.fillLower);
	ui->transparencyWidget->setOpacityPercent(s.opacity);
}

void Post2dWindowContourSettingDialog::setColorBarTitleMap(const QMap<std::string, QString>& titlemap)
{
	m_colorBarTitleMap = titlemap;
}

Post2dWindowContourSetting Post2dWindowContourSettingDialog::setting() const
{
	Post2dWindowContourSetting ret = m_setting;

	ret.target = target().c_str();
	ret.contour = ui->contourWidget->contour();
	ret.numberOfDivisions = ui->colormapWidget->divisionNumber();
	ret.fillUpper = ui->colormapWidget->fillUpper();
	ret.fillLower = ui->colormapWidget->fillLower();
	ret.opacity = ui->transparencyWidget->opacityPercent();

	return ret;
}

QString Post2dWindowContourSettingDialog::scalarBarTitle()
{
	return m_colorBarTitleMap[target()];
}

std::string Post2dWindowContourSettingDialog::target() const
{
	return m_solutions.at(ui->physicalValueComboBox->currentIndex());
}

const LookupTableContainer& Post2dWindowContourSettingDialog::lookupTable()
{
	return m_lookupTable;
}

void Post2dWindowContourSettingDialog::targetChanged(int index)
{
	std::string sol = m_solutions.at(index);
	LookupTableContainer* c;
	if (this->m_location == Vertex) {
		c = m_gridTypeDataItem->nodeLookupTable(sol);
	} else if (this->m_location == CellCenter) {
		c = m_gridTypeDataItem->cellLookupTable(sol);
	} else {
		Q_ASSERT(false);
	}

	m_lookupTable = *c;
	ui->colormapWidget->setContainer(&m_lookupTable);
}

void Post2dWindowContourSettingDialog::accept()
{
	if ((! ui->colormapWidget->autoRange()) &&
			(ui->colormapWidget->upperLimit() < ui->colormapWidget->lowerLimit())) {
		QMessageBox::critical(this, tr("Error"), tr("For value range, max value is smaller than min value."));
		return;
	}

	ui->colormapWidget->save();
	QDialog::accept();
}

void Post2dWindowContourSettingDialog::showRegionDialog()
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

void Post2dWindowContourSettingDialog::showScalarBarDialog()
{
	ScalarBarDialog dialog(this);
	dialog.setSetting(m_setting.scalarBarSetting);
	dialog.setTitle(m_colorBarTitleMap[target()]);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	m_setting.scalarBarSetting = dialog.setting();
	m_colorBarTitleMap[target()] = dialog.title();
}
