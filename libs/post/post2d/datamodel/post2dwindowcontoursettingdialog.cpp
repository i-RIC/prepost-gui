#include "ui_post2dwindowcontoursettingdialog.h"

#include "../post2dgridregionselectdialog.h"
#include "post2dwindowcontoursettingdialog.h"
#include "post2dwindowgridtypedataiteminterface.h"

#include <guibase/scalarbardialog.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>

#include <QMessageBox>

#include <vtkPointData.h>
#include <vtkStructuredGrid.h>

Post2dWindowContourSettingDialog::Post2dWindowContourSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Post2dWindowContourSettingDialog)
{
	m_activeAvailable = true;
	m_unstructured = false;

	ui->setupUi(this);
	ui->contourWidget->hidePoints();

	connect(ui->physicalValueComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(solutionChanged(int)));
	connect(ui->regionSettingButton, SIGNAL(clicked()), this, SLOT(showRegionDialog()));
	connect(ui->colorbarSettingButton, SIGNAL(clicked()), this, SLOT(showScalarBarDialog()));
	connect(ui->contourWidget, SIGNAL(contourChanged(ContourSettingWidget::Contour)), ui->colormapWidget, SLOT(setContourSetting(ContourSettingWidget::Contour)));
}

Post2dWindowContourSettingDialog::~Post2dWindowContourSettingDialog()
{
	delete ui;
}

void Post2dWindowContourSettingDialog::setZoneData(PostZoneDataContainer* zoneData)
{
	vtkPointData* pd = zoneData->data()->GetPointData();
	int num = pd->GetNumberOfArrays();
	ui->physicalValueComboBox->clear();
	m_solutions.clear();
	ui->physicalValueComboBox->blockSignals(true);
	SolverDefinitionGridType* gtype = m_gridTypeDataItem->gridType();

	for (int i = 0; i < num; ++i) {
		vtkAbstractArray* tmparray = pd->GetArray(i);
		if (tmparray == nullptr) {continue;}
		QString name = tmparray->GetName();
		if (tmparray->GetNumberOfComponents() > 1) {
			// vector attributes.
			continue;
		}

		ui->physicalValueComboBox->addItem(gtype->solutionCaption(name));
		m_solutions.append(name);
	}
	ui->physicalValueComboBox->blockSignals(false);
	if (gtype->defaultGridType() == SolverDefinitionGridType::gtUnstructured2DGrid) {
		// unstructured grid!
		m_unstructured = true;
	} else {
		// structured grid!
		vtkStructuredGrid* g = dynamic_cast<vtkStructuredGrid*>(zoneData->data());
		g->GetDimensions(m_gridDims);
	}
}

void Post2dWindowContourSettingDialog::setCurrentSolution(QString sol)
{
	int index = m_solutions.indexOf(sol);
	if (index == -1) {
		// not set yet. select the first one.
		index = 0;
	}
	ui->physicalValueComboBox->setCurrentIndex(index);
	solutionChanged(index);
}

void Post2dWindowContourSettingDialog::setContour(ContourSettingWidget::Contour c)
{
	ui->contourWidget->setContour(c);
}

void Post2dWindowContourSettingDialog::setNumberOfDivision(int div)
{
	ui->colormapWidget->setDivisionNumber(div);
}

void Post2dWindowContourSettingDialog::setGridTypeDataItem(Post2dWindowGridTypeDataItemInterface* item)
{
	m_gridTypeDataItem = item;
}

QString Post2dWindowContourSettingDialog::currentSolution()
{
	return m_solutions.at(ui->physicalValueComboBox->currentIndex());
}

ContourSettingWidget::Contour Post2dWindowContourSettingDialog::contour()
{
	return ui->contourWidget->contour();
}

int Post2dWindowContourSettingDialog::numberOfDivision()
{
	return ui->colormapWidget->divisionNumber();
}

const LookupTableContainer& Post2dWindowContourSettingDialog::lookupTable()
{
	return m_lookupTable;
}

void Post2dWindowContourSettingDialog::solutionChanged(int index)
{
	QString sol = m_solutions.at(index);
	LookupTableContainer* c = m_gridTypeDataItem->lookupTable(sol);
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

void Post2dWindowContourSettingDialog::setRegionMode(StructuredGridRegion::RegionMode mode)
{
	m_regionMode = mode;
}

StructuredGridRegion::RegionMode Post2dWindowContourSettingDialog::regionMode()
{
	return m_regionMode;
}

void Post2dWindowContourSettingDialog::setRange(StructuredGridRegion::Range2d range)
{
	m_region = range;
}

StructuredGridRegion::Range2d Post2dWindowContourSettingDialog::range()
{
	return m_region;
}

void Post2dWindowContourSettingDialog::setFillUpper(bool fill)
{
	ui->colormapWidget->setFillUpper(fill);
}

void Post2dWindowContourSettingDialog::setFillLower(bool fill)
{
	ui->colormapWidget->setFillLower(fill);
}

bool Post2dWindowContourSettingDialog::fillUpper()
{
	return ui->colormapWidget->fillUpper();
}

bool Post2dWindowContourSettingDialog::fillLower()
{
	return ui->colormapWidget->fillLower();
}

void Post2dWindowContourSettingDialog::setColorBarTitleMap(const QMap<QString, QString>& titlemap)
{
	m_colorBarTitleMap = titlemap;
}

void Post2dWindowContourSettingDialog::setScalarBarSetting(const ScalarBarSetting& setting)
{
	m_scalarBarSetting = setting;
}

void Post2dWindowContourSettingDialog::setTitleTextSetting(const vtkTextPropertySettingContainer& cont)
{
	m_titleTextSetting = cont;
}

void Post2dWindowContourSettingDialog::setLabelTextSetting(const vtkTextPropertySettingContainer& cont)
{
	m_labelTextSetting = cont;
}

QString Post2dWindowContourSettingDialog::scalarBarTitle()
{
	return m_colorBarTitleMap[currentSolution()];
}

void Post2dWindowContourSettingDialog::setOpacityPercent(int o)
{
	ui->transparencyWidget->setOpacity(o);
}

int Post2dWindowContourSettingDialog::opacityPercent()
{
	return ui->transparencyWidget->opacity();
}

void Post2dWindowContourSettingDialog::disableActive()
{
	m_activeAvailable = false;
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
	dialog.setGridSize(m_gridDims[0], m_gridDims[1]);
	dialog.setRegionMode(m_regionMode);
	dialog.setRegion(m_region);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}
	m_regionMode = dialog.regionMode();
	m_region = dialog.region();
}

void Post2dWindowContourSettingDialog::showScalarBarDialog()
{
	ScalarBarDialog dialog(this);
	dialog.setTitle(m_colorBarTitleMap[currentSolution()]);
	dialog.setSetting(m_scalarBarSetting);
	dialog.setTitleTextSetting(m_titleTextSetting);
	dialog.setLabelTextSetting(m_labelTextSetting);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	m_colorBarTitleMap[currentSolution()] = dialog.title();
	m_scalarBarSetting = dialog.setting();
	m_titleTextSetting = dialog.titleTextSetting();
	m_labelTextSetting = dialog.labelTextSetting();
}

void Post2dWindowContourSettingDialog::hideOpacity()
{
	ui->transparencyWidget->hide();
	QSize size = minimumSize();
	resize(size);
}
