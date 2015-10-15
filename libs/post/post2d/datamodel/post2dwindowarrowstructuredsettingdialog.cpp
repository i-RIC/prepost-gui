#include "ui_post2dwindowarrowstructuredsettingdialog.h"

#include "../post2dgridregionselectdialog.h"
#include "post2dwindowarrowstructuredsettingdialog.h"

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


void Post2dWindowArrowStructuredSettingDialog::setSettings(const Post2dWindowNodeVectorArrowGroupDataItem::Setting& s, const Post2dWindowNodeVectorArrowGroupStructuredDataItem::Setting& sts)
{
	m_setting = s;
	m_stSetting = sts;
	// currentSolution
	int index = m_solutions.indexOf(s.currentSolution);
	if (index == -1) {index = 0;}
	ui->solutionComboBox->setCurrentIndex(index);

	// scalarValue
	index = m_scalars.indexOf(s.scalarValueName);
	if (index == -1) { index = 0; }
	ui->scalarComboBox->setCurrentIndex(index);

	// color
	ui->colorEditWidget->setColor(s.color);

	// mapping
	switch (Post2dWindowNodeVectorArrowGroupDataItem::Mapping(s.mapping)) {
	case Post2dWindowNodeVectorArrowGroupDataItem::Specific:
		ui->specificRadioButton->setChecked(true);
		break;
	case Post2dWindowNodeVectorArrowGroupDataItem::Scalar:
		ui->scalarRadioButton->setChecked(true);
		break;
	}

	// arrowSetting
	ui->arrowSizeSpinBox->setValue(s.arrowSetting.arrowSize());
	ui->lineWidthSpinBox->setValue(s.arrowSetting.lineWidth());

	// lengthMode
	if (s.lengthMode == Post2dWindowNodeVectorArrowGroupDataItem::lenAuto) {
		ui->lengthAutoCheckBox->setChecked(true);
	} else {
		ui->lengthAutoCheckBox->setChecked(false);
	}

	// standardValue
	ui->stdValueSpinBox->setValue(s.standardValue);

	// legendLength
	ui->legendLengthSpinBox->setValue(s.legendLength);

	// minimumValue
	return ui->minValueSpinBox->setValue(s.minimumValue);

	// samplingRates
	ui->iSamplingRateSpinBox->setValue(sts.iSampleRate);
	ui->jSamplingRateSpinBox->setValue(sts.jSampleRate);
	ui->samplingSkipRadioButton->setChecked(true);
	if (sts.iSampleRate == 1 && sts.jSampleRate == 1) {
		ui->samplingAllRadioButton->setChecked(true);
	}
}

Post2dWindowNodeVectorArrowGroupDataItem::Setting Post2dWindowArrowStructuredSettingDialog::setting() const
{
	Post2dWindowNodeVectorArrowGroupDataItem::Setting ret = m_setting;

	// currentSolution
	int index = ui->solutionComboBox->currentIndex();
	ret.currentSolution = m_solutions.at(index);

	// scalarValue
	index = ui->scalarComboBox->currentIndex();
	ret.scalarValueName = m_scalars.at(index);

	// color
	ret.color = ui->colorEditWidget->color();

	// mapping
	if (ui->specificRadioButton->isChecked()) {ret.mapping = Post2dWindowNodeVectorArrowGroupDataItem::Specific;}
	if (ui->scalarRadioButton->isChecked()) {ret.mapping = Post2dWindowNodeVectorArrowGroupDataItem::Scalar;}

	// arrowSetting
	ret.arrowSetting.setArrowSize(ui->arrowSizeSpinBox->value());
	ret.arrowSetting.setLineWidth(ui->lineWidthSpinBox->value());

	// lengthMode
	if (ui->lengthAutoCheckBox->isChecked()) {
		ret.lengthMode = Post2dWindowNodeVectorArrowGroupDataItem::lenAuto;
	} else {
		ret.lengthMode = Post2dWindowNodeVectorArrowGroupDataItem::lenCustom;
	}

	// standardValue
	ret.standardValue = ui->stdValueSpinBox->value();

	// legendLength;
	ret.legendLength = ui->legendLengthSpinBox->value();

	// minimumValue
	ret.minimumValue = ui->minValueSpinBox->value();

	return ret;
}

Post2dWindowNodeVectorArrowGroupStructuredDataItem::Setting Post2dWindowArrowStructuredSettingDialog::stSetting() const
{
	Post2dWindowNodeVectorArrowGroupStructuredDataItem::Setting ret = m_stSetting;

	// samplingRates
	ret.iSampleRate = ui->iSamplingRateSpinBox->value();
	ret.jSampleRate = ui->jSamplingRateSpinBox->value();

	return ret;
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
	dialog.setRegionMode(m_setting.regionMode);
	dialog.setRegion(m_stSetting.range);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}
	m_setting.regionMode = dialog.regionMode();
	m_stSetting.range = dialog.region();
}

void Post2dWindowArrowStructuredSettingDialog::disableActive()
{
	m_activeAvailable = false;
}
