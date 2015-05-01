#include "../post2dgridregionselectdialog.h"
#include "post2dwindowarrowstructuredsettingdialog.h"
#include "ui_post2dwindowarrowstructuredsettingdialog.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/arrowsettingcontainer.h>

#include <QtGlobal>

#include <vtkPointData.h>
#include <vtkStructuredGrid.h>

Post2dWindowArrowStructuredSettingDialog::Post2dWindowArrowStructuredSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Post2dWindowArrowStructuredSettingDialog)
{
	ui->setupUi(this);
	ui->colorbarSettingButton->hide();
	connect(ui->samplingAllRadioButton, SIGNAL(toggled(bool)), this, SLOT(samplingAllToggle(bool)));
	connect(ui->regionSettingButton, SIGNAL(clicked()), this, SLOT(showRegionDialog()));
	m_activeAvailable = true;
}

Post2dWindowArrowStructuredSettingDialog::~Post2dWindowArrowStructuredSettingDialog()
{
	delete ui;
}

void Post2dWindowArrowStructuredSettingDialog::setZoneData(PostZoneDataContainer* zoneData)
{
	vtkStructuredGrid* g = dynamic_cast<vtkStructuredGrid*>(zoneData->data());
	g->GetDimensions(m_gridDims);
	setupSolutionComboBox(zoneData);
}

void Post2dWindowArrowStructuredSettingDialog::setSolution(const QString& sol)
{
	int index = m_solutions.indexOf(sol);
	if (index == -1) {index = 0;}
	ui->solutionComboBox->setCurrentIndex(index);
}

const QString Post2dWindowArrowStructuredSettingDialog::solution()
{
	int index = ui->solutionComboBox->currentIndex();
	return m_solutions.at(index);
}

void Post2dWindowArrowStructuredSettingDialog::setScalarValue(const QString& scalar)
{
	int index = m_scalars.indexOf(scalar);
	if (index == -1) { index = 0; }
	ui->scalarComboBox->setCurrentIndex(index);
}

const QString Post2dWindowArrowStructuredSettingDialog::scalarValue()
{
	int index = ui->scalarComboBox->currentIndex();
	return m_scalars.at(index);
}

void Post2dWindowArrowStructuredSettingDialog::setColor(const QColor& color)
{
	ui->colorEditWidget->setColor(color);
}

const QColor Post2dWindowArrowStructuredSettingDialog::color() const
{
	return ui->colorEditWidget->color();
}

void Post2dWindowArrowStructuredSettingDialog::setMapping(Post2dWindowNodeVectorArrowGroupDataItem::Mapping mapping)
{
	switch (mapping) {
	case Post2dWindowNodeVectorArrowGroupDataItem::Specific:
		ui->specificRadioButton->setChecked(true);
		break;
	case Post2dWindowNodeVectorArrowGroupDataItem::Scalar:
		ui->scalarRadioButton->setChecked(true);
		break;
	}
}

Post2dWindowNodeVectorArrowGroupDataItem::Mapping Post2dWindowArrowStructuredSettingDialog::mapping()
{
	if (ui->specificRadioButton->isChecked()) { return Post2dWindowNodeVectorArrowGroupDataItem::Specific; }
	if (ui->scalarRadioButton->isChecked()) { return Post2dWindowNodeVectorArrowGroupDataItem::Scalar; }
	// default
	return Post2dWindowNodeVectorArrowGroupDataItem::Specific;
}

void Post2dWindowArrowStructuredSettingDialog::setArrowSetting(const ArrowSettingContainer& acon)
{
	ui->arrowSizeSpinBox->setValue(acon.arrowSize());
	ui->lineWidthSpinBox->setValue(acon.lineWidth());
}

ArrowSettingContainer Post2dWindowArrowStructuredSettingDialog::arrowSetting()
{
	ArrowSettingContainer c;
	c.setArrowSize(ui->arrowSizeSpinBox->value());
	c.setLineWidth(ui->lineWidthSpinBox->value());
	return c;
}

void Post2dWindowArrowStructuredSettingDialog::setSamplingRates(int irate, int jrate)
{
	ui->iSamplingRateSpinBox->setValue(irate);
	ui->jSamplingRateSpinBox->setValue(jrate);
	if (irate == 1 && jrate == 1) {
		ui->samplingAllRadioButton->setChecked(true);
	} else {
		ui->samplingSkipRadioButton->setChecked(true);
	}
}

int Post2dWindowArrowStructuredSettingDialog::iSamplingRate()
{
	return ui->iSamplingRateSpinBox->value();
}

int Post2dWindowArrowStructuredSettingDialog::jSamplingRate()
{
	return ui->jSamplingRateSpinBox->value();
}

void Post2dWindowArrowStructuredSettingDialog::setRegionMode(StructuredGridRegion::RegionMode regionMode)
{
	m_regionMode = regionMode;
}

StructuredGridRegion::RegionMode Post2dWindowArrowStructuredSettingDialog::regionMode() const
{
	return m_regionMode;
}

void Post2dWindowArrowStructuredSettingDialog::setRange(const StructuredGridRegion::Range2d& range)
{
	m_range = range;
}

StructuredGridRegion::Range2d Post2dWindowArrowStructuredSettingDialog::range()
{
	return m_range;
}

void Post2dWindowArrowStructuredSettingDialog::setupSolutionComboBox(PostZoneDataContainer* zoneData)
{
	vtkPointData* pd = zoneData->data()->GetPointData();
	SolverDefinitionGridType* gt = zoneData->gridType();
	int num = pd->GetNumberOfArrays();
	ui->solutionComboBox->blockSignals(true);
	for (int i = 0; i < num; ++i) {
		vtkAbstractArray* tmparray = pd->GetArray(i);
		if (tmparray == nullptr) {continue;}
		QString name = tmparray->GetName();
		if (pd->GetArray(i)->GetNumberOfComponents() <= 1) {
			// scalar attributes.
			ui->scalarComboBox->addItem(gt->solutionCaption(name));
			m_scalars.append(name);
		} else {
			// vector attributes.
			ui->solutionComboBox->addItem(gt->solutionCaption(name));
			m_solutions.append(name);
		}
	}
	ui->solutionComboBox->blockSignals(false);
	if (m_solutions.count() <= 1) {
		ui->physValLabel->hide();
		ui->solutionComboBox->hide();
	}
}

void Post2dWindowArrowStructuredSettingDialog::samplingAllToggle(bool toggled)
{
	if (! toggled) {return;}
	ui->iSamplingRateSpinBox->setValue(1);
	ui->jSamplingRateSpinBox->setValue(1);
}

void Post2dWindowArrowStructuredSettingDialog::showRegionDialog()
{
	Post2dGridRegionSelectDialog dialog(this);
	if (! m_activeAvailable) {
		dialog.disableActive();
	}
	dialog.setGridSize(m_gridDims[0], m_gridDims[1]);
	dialog.setRegionMode(m_regionMode);
	dialog.setRegion(m_range);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}
	m_regionMode = dialog.regionMode();
	m_range = dialog.region();
}

void Post2dWindowArrowStructuredSettingDialog::disableActive()
{
	m_activeAvailable = false;
}

void Post2dWindowArrowStructuredSettingDialog::setLengthMode(Post2dWindowNodeVectorArrowGroupDataItem::LengthMode lm)
{
	if (lm == Post2dWindowNodeVectorArrowGroupDataItem::lenAuto) {
		ui->lengthAutoCheckBox->setChecked(true);
	} else {
		ui->lengthAutoCheckBox->setChecked(false);
	}
}

Post2dWindowNodeVectorArrowGroupDataItem::LengthMode Post2dWindowArrowStructuredSettingDialog::lengthMode()
{
	if (ui->lengthAutoCheckBox->isChecked()) {
		return Post2dWindowNodeVectorArrowGroupDataItem::lenAuto;
	} else {
		return Post2dWindowNodeVectorArrowGroupDataItem::lenCustom;
	}
}

void Post2dWindowArrowStructuredSettingDialog::setStandardValue(double stdVal)
{
	ui->stdValueSpinBox->setValue(stdVal);
}

double Post2dWindowArrowStructuredSettingDialog::standardValue()
{
	return ui->stdValueSpinBox->value();
}

void Post2dWindowArrowStructuredSettingDialog::setLegendLength(int len)
{
	ui->legendLengthSpinBox->setValue(len);
}

int Post2dWindowArrowStructuredSettingDialog::legendLength()
{
	return ui->legendLengthSpinBox->value();
}

void Post2dWindowArrowStructuredSettingDialog::setMinimumValue(double minVal)
{
	return ui->minValueSpinBox->setValue(minVal);
}

double Post2dWindowArrowStructuredSettingDialog::minimumValue()
{
	return ui->minValueSpinBox->value();
}
