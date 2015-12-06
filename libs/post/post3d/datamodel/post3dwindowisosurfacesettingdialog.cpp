#include "ui_post3dwindowisosurfacesettingdialog.h"

#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowisosurfacesettingdialog.h"

#include <guibase/comboboxtool.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guibase/structuredgridregion.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>

#include <vtkPointData.h>
#include <vtkStructuredGrid.h>

Post3dWindowIsosurfaceSettingDialog::Post3dWindowIsosurfaceSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Post3dWindowIsosurfaceSettingDialog)
{
	ui->setupUi(this);
	ui->enabledCheckBox->hide();

	ui->minValueEdit->setDisabled(true);
	ui->maxValueEdit->setDisabled(true);

	connect(ui->enabledCheckBox, SIGNAL(clicked(bool)), this, SLOT(setEnableCheckBox(bool)));
	connect(ui->physicalValueComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(targetChanged(int)));
	connect(ui->fullRangeCheckBox, SIGNAL(clicked(bool)), this, SLOT(fullRangeChanged(bool)));
	connect(ui->iminSlider, SIGNAL(valueChanged(int)), this, SLOT(iMinChanged(int)));
	connect(ui->imaxSlider, SIGNAL(valueChanged(int)), this, SLOT(iMaxChanged(int)));
	connect(ui->jminSlider, SIGNAL(valueChanged(int)), this, SLOT(jMinChanged(int)));
	connect(ui->jmaxSlider, SIGNAL(valueChanged(int)), this, SLOT(jMaxChanged(int)));
	connect(ui->kminSlider, SIGNAL(valueChanged(int)), this, SLOT(kMinChanged(int)));
	connect(ui->kmaxSlider, SIGNAL(valueChanged(int)), this, SLOT(kMaxChanged(int)));
}

Post3dWindowIsosurfaceSettingDialog::~Post3dWindowIsosurfaceSettingDialog()
{
	delete ui;
}

void Post3dWindowIsosurfaceSettingDialog::setZoneData(PostZoneDataContainer* zoneData)
{
	vtkPointData* pd = zoneData->data()->GetPointData();

	m_targets = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(pd);
	ComboBoxTool::setupItems(m_gridTypeDataItem->gridType()->solutionCaptions(m_targets), ui->physicalValueComboBox);

	vtkStructuredGrid* g = dynamic_cast<vtkStructuredGrid*>(zoneData->data());
	int dims[3];
	g->GetDimensions(dims);
	ui->iminSlider->setRange(1, dims[0]);
	ui->imaxSlider->setRange(1, dims[0]);
	ui->jminSlider->setRange(1, dims[1]);
	ui->jmaxSlider->setRange(1, dims[1]);
	ui->kminSlider->setRange(1, dims[2]);
	ui->kmaxSlider->setRange(1, dims[2]);
}

void Post3dWindowIsosurfaceSettingDialog::setEnabled(bool enabled)
{
	ui->enabledCheckBox->setChecked(enabled);
	setEnableCheckBox(enabled);
}

void Post3dWindowIsosurfaceSettingDialog::setTarget(const std::string& target)
{
	auto it = std::find(m_targets.begin(), m_targets.end(), target);
	if (it == m_targets.end()) {it = m_targets.begin();}
	ui->physicalValueComboBox->setCurrentIndex(it - m_targets.begin());
	targetChanged(it - m_targets.begin());
}

void Post3dWindowIsosurfaceSettingDialog::setGridTypeDataItem(Post3dWindowGridTypeDataItem* item)
{
	m_gridTypeDataItem = item;
}

bool Post3dWindowIsosurfaceSettingDialog::enabled()
{
	return ui->enabledCheckBox->isChecked();
}

std::string Post3dWindowIsosurfaceSettingDialog::target() const
{
	return m_targets.at(ui->physicalValueComboBox->currentIndex());
}

void Post3dWindowIsosurfaceSettingDialog::setEnableCheckBox(bool enable)
{
	ui->physicalValueComboBox->setEnabled(enable);
}

void Post3dWindowIsosurfaceSettingDialog::targetChanged(int index)
{
	std::string target = m_targets.at(index);
	LookupTableContainer* c = m_gridTypeDataItem->lookupTable(target);

	ui->minValueEdit->setValue(c->autoMin());
	ui->maxValueEdit->setValue(c->autoMax());
}

void Post3dWindowIsosurfaceSettingDialog::accept()
{
	QDialog::accept();
}

void Post3dWindowIsosurfaceSettingDialog::setFullRange(bool full)
{
	ui->fullRangeCheckBox->setChecked(full);
	fullRangeChanged(full);
}

bool Post3dWindowIsosurfaceSettingDialog::fullRange() const
{
	return ui->fullRangeCheckBox->isChecked();
}

void Post3dWindowIsosurfaceSettingDialog::setRange(StructuredGridRegion::Range3d range)
{
	if (ui->fullRangeCheckBox->isChecked()) {return;}
	ui->iminSlider->setValue(range.iMin + 1);
	ui->imaxSlider->setValue(range.iMax + 1);
	ui->jminSlider->setValue(range.jMin + 1);
	ui->jmaxSlider->setValue(range.jMax + 1);
	ui->kminSlider->setValue(range.kMin + 1);
	ui->kmaxSlider->setValue(range.kMax + 1);
}

StructuredGridRegion::Range3d Post3dWindowIsosurfaceSettingDialog::range() const
{
	StructuredGridRegion::Range3d range;
	range.iMin = ui->iminSlider->value() - 1;
	range.iMax = ui->imaxSlider->value() - 1;
	range.jMin = ui->jminSlider->value() - 1;
	range.jMax = ui->jmaxSlider->value() - 1;
	range.kMin = ui->kminSlider->value() - 1;
	range.kMax = ui->kmaxSlider->value() - 1;
	return range;
}

void Post3dWindowIsosurfaceSettingDialog::setIsoValue(double val)
{
	ui->isoValueEdit->setValue(val);
}

double Post3dWindowIsosurfaceSettingDialog::isoValue() const
{
	return ui->isoValueEdit->text().toDouble();
}

void Post3dWindowIsosurfaceSettingDialog::fullRangeChanged(bool full)
{
	ui->iminSlider->setDisabled(full);
	ui->imaxSlider->setDisabled(full);
	ui->jminSlider->setDisabled(full);
	ui->jmaxSlider->setDisabled(full);
	ui->kminSlider->setDisabled(full);
	ui->kmaxSlider->setDisabled(full);
	if (full) {
		ui->iminSlider->setValue(1);
		ui->imaxSlider->setValue(ui->imaxSlider->maximum());
		ui->jminSlider->setValue(1);
		ui->jmaxSlider->setValue(ui->jmaxSlider->maximum());
		ui->kminSlider->setValue(1);
		ui->kmaxSlider->setValue(ui->kmaxSlider->maximum());
	}
}

void Post3dWindowIsosurfaceSettingDialog::iMinChanged(int min)
{
	if (ui->imaxSlider->value() < min) {
		ui->imaxSlider->setValue(min);
	}
}

void Post3dWindowIsosurfaceSettingDialog::iMaxChanged(int max)
{
	if (ui->iminSlider->value() > max) {
		ui->iminSlider->setValue(max);
	}
}

void Post3dWindowIsosurfaceSettingDialog::jMinChanged(int min)
{
	if (ui->jmaxSlider->value() < min) {
		ui->jmaxSlider->setValue(min);
	}
}

void Post3dWindowIsosurfaceSettingDialog::jMaxChanged(int max)
{
	if (ui->jminSlider->value() > max) {
		ui->jminSlider->setValue(max);
	}
}

void Post3dWindowIsosurfaceSettingDialog::kMinChanged(int min)
{
	if (ui->kmaxSlider->value() < min) {
		ui->kmaxSlider->setValue(min);
	}
}

void Post3dWindowIsosurfaceSettingDialog::kMaxChanged(int max)
{
	if (ui->kminSlider->value() > max) {
		ui->kminSlider->setValue(max);
	}
}

void Post3dWindowIsosurfaceSettingDialog::setColor(const QColor& color)
{
	ui->colorEditWidget->setColor(color);
}

const QColor Post3dWindowIsosurfaceSettingDialog::color() const
{
	return ui->colorEditWidget->color();
}
