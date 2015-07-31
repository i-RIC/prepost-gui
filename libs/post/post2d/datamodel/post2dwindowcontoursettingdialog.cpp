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

void Post2dWindowContourSettingDialog::setGridTypeDataItem(Post2dWindowGridTypeDataItemInterface* item)
{
	m_gridTypeDataItem = item;
}

void Post2dWindowContourSettingDialog::disableActive()
{
	m_activeAvailable = false;
}

void Post2dWindowContourSettingDialog::hideOpacity()
{
	ui->transparencyWidget->hide();
	QSize size = minimumSize();
	resize(size);
}

void Post2dWindowContourSettingDialog::setSetting(const Post2dWindowNodeScalarGroupDataItem::Setting& s)
{
	// currentSolution
	int index = m_solutions.indexOf(s.currentSolution);
	if (index == -1) {
		// not set yet. select the first one.
		index = 0;
	}
	ui->physicalValueComboBox->setCurrentIndex(index);
	solutionChanged(index);

	// contour
	ui->contourWidget->setContour(s.contour);

	// numberOfDivision
	ui->colormapWidget->setDivisionNumber(s.numberOfDivisions);

	// fillUpper
	ui->colormapWidget->setFillUpper(s.fillUpper);

	// fillLower
	ui->colormapWidget->setFillLower(s.fillLower);

	// opacity
	ui->transparencyWidget->setOpacity(s.opacity);

}

void Post2dWindowContourSettingDialog::setColorBarTitleMap(const QMap<QString, QString>& titlemap)
{
	m_colorBarTitleMap = titlemap;
}

Post2dWindowNodeScalarGroupDataItem::Setting Post2dWindowContourSettingDialog::setting() const
{
	Post2dWindowNodeScalarGroupDataItem::Setting ret = m_setting;
	// solution
	ret.currentSolution = currentSolution();

	// contour
	ret.contour = ui->contourWidget->contour();

	// numberOfDivision
	ret.numberOfDivisions = ui->colormapWidget->divisionNumber();

	// fillUpper
	ret.fillUpper = ui->colormapWidget->fillUpper();

	// fillLower
	ret.fillLower = ui->colormapWidget->fillLower();

	// opacity
	ret.opacity = ui->transparencyWidget->opacity();

	return ret;
}

QString Post2dWindowContourSettingDialog::scalarBarTitle()
{
	return m_colorBarTitleMap[currentSolution()];
}

QString Post2dWindowContourSettingDialog::currentSolution() const
{
	return m_solutions.at(ui->physicalValueComboBox->currentIndex());
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
	dialog.setTitle(m_colorBarTitleMap[currentSolution()]);
	dialog.setSetting(m_setting.scalarBarSetting);
	dialog.setTitleTextSetting(m_setting.titleTextSetting);
	dialog.setLabelTextSetting(m_setting.labelTextSetting);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}

	m_colorBarTitleMap[currentSolution()] = dialog.title();
	m_setting.scalarBarSetting = dialog.setting();
	m_setting.titleTextSetting = dialog.titleTextSetting();
	m_setting.labelTextSetting = dialog.labelTextSetting();
}
