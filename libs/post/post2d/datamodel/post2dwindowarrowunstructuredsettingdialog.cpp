#include "ui_post2dwindowarrowunstructuredsettingdialog.h"

#include "../post2dgridregionselectdialog.h"
#include "post2dwindowarrowunstructuredsettingdialog.h"

#include <guibase/comboboxtool.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>

#include <QtGlobal>

#include <vtkPointData.h>
#include <vtkStructuredGrid.h>

Post2dWindowArrowUnstructuredSettingDialog::Post2dWindowArrowUnstructuredSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Post2dWindowArrowUnstructuredSettingDialog)
{
	ui->setupUi(this);

	connect(ui->specificRadioButton, SIGNAL(toggled(bool)), ui->colorEditWidget, SLOT(setEnabled(bool)));
	connect(ui->scalarRadioButton, SIGNAL(toggled(bool)), ui->scalarComboBox, SLOT(setEnabled(bool)));
	connect(ui->regionSettingButton, SIGNAL(clicked()), this, SLOT(showRegionDialog()));
}

Post2dWindowArrowUnstructuredSettingDialog::~Post2dWindowArrowUnstructuredSettingDialog()
{
	delete ui;
}

void Post2dWindowArrowUnstructuredSettingDialog::setZoneData(PostZoneDataContainer* zoneData)
{
	vtkPointSet* p = zoneData->data();
	ui->samplingNumberSpinBox->setMaximum(p->GetNumberOfPoints());
	setupSolutionComboBox(zoneData);
}

void Post2dWindowArrowUnstructuredSettingDialog::disableActive()
{
	m_activeAvailable = false;
}

void Post2dWindowArrowUnstructuredSettingDialog::setSettings(const Post2dWindowNodeVectorArrowGroupDataItem::Setting& s, const Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::Setting& unss)
{
	// solution
	auto it = std::find(m_solutions.begin(), m_solutions.end(), iRIC::toStr(s.target));
	if (it == m_solutions.end()) {it = m_solutions.begin();}
	ui->solutionComboBox->setCurrentIndex(it - m_solutions.begin());

	// scalarValue
	it = std::find(m_scalars.begin(), m_scalars.end(), iRIC::toStr(s.scalarValueName));
	if (it == m_scalars.end()) {it = m_scalars.begin();}
	ui->scalarComboBox->setCurrentIndex(it - m_scalars.begin());

	// color
	ui->colorEditWidget->setColor(s.color);

	// mappingMode
	switch (Post2dWindowNodeVectorArrowGroupDataItem::Mapping(s.mapping)) {
	case Post2dWindowNodeVectorArrowGroupDataItem::Specific:
		ui->specificRadioButton->setChecked(true);
		break;
	case Post2dWindowNodeVectorArrowGroupDataItem::Scalar:
		ui->scalarRadioButton->setChecked(true);
		break;
	}

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
	ui->minValueSpinBox->setValue(s.minimumValue);

	// arrowSetting
	ui->arrowSizeSpinBox->setValue(s.arrowSetting.arrowSize());
	ui->lineWidthSpinBox->setValue(s.arrowSetting.lineWidth());

	// samplingMode
	if (unss.samplingMode == Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::smAll) {
		ui->samplingAllRadioButton->setChecked(true);
	} else if (unss.samplingMode == Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::smRate) {
		ui->samplingRateRadioButton->setChecked(true);
	} else {
		ui->samplingNumberRadioButton->setChecked(true);
	}

	// samplingRate
	ui->samplingRateSpinBox->setValue(unss.samplingRate);

	// samplingNumber
	ui->samplingNumberSpinBox->setValue(unss.samplingNumber);

}

Post2dWindowNodeVectorArrowGroupDataItem::Setting Post2dWindowArrowUnstructuredSettingDialog::setting() const
{
	Post2dWindowNodeVectorArrowGroupDataItem::Setting ret = m_setting;

	// solution
	int index = ui->solutionComboBox->currentIndex();
	ret.target = m_solutions.at(index).c_str();

	// scalarValue
	index = ui->scalarComboBox->currentIndex();
	ret.scalarValueName = m_scalars.at(index).c_str();

	// color
	ret.color = ui->colorEditWidget->color();

	// mapping
	if (ui->specificRadioButton->isChecked()) {ret.mapping = Post2dWindowNodeVectorArrowGroupDataItem::Specific;}
	if (ui->scalarRadioButton->isChecked()) {ret.mapping = Post2dWindowNodeVectorArrowGroupDataItem::Scalar;}

	// lengthMode
	if (ui->lengthAutoCheckBox->isChecked()) {
		ret.lengthMode = Post2dWindowNodeVectorArrowGroupDataItem::lenAuto;
	} else {
		ret.lengthMode = Post2dWindowNodeVectorArrowGroupDataItem::lenCustom;
	}

	// standardValue
	ret.standardValue = ui->stdValueSpinBox->value();

	// legendLength
	ret.legendLength = ui->legendLengthSpinBox->value();

	// minimumValue
	ret.minimumValue = ui->minValueSpinBox->value();

	// arrowSetting
	ret.arrowSetting.setArrowSize(ui->arrowSizeSpinBox->value());
	ret.arrowSetting.setLineWidth(ui->lineWidthSpinBox->value());

	return ret;
}

Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::Setting Post2dWindowArrowUnstructuredSettingDialog::unsSetting() const
{
	Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::Setting ret = m_unsSetting;

	// samplingMode
	if (ui->samplingAllRadioButton->isChecked()) {
		ret.samplingMode = Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::smAll;
	} else if (ui->samplingRateRadioButton->isChecked()) {
		ret.samplingMode = Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::smRate;
	} else {
		ret.samplingMode = Post2dWindowNodeVectorArrowGroupUnstructuredDataItem::smNumber;
	}

	// samplingRate
	ret.samplingRate = ui->samplingRateSpinBox->value();

	// samplingNumber
	ret.samplingNumber = ui->samplingNumberSpinBox->value();
	return ret;
}

void Post2dWindowArrowUnstructuredSettingDialog::setupSolutionComboBox(PostZoneDataContainer* zoneData)
{
	vtkPointData* pd = zoneData->data()->GetPointData();
	SolverDefinitionGridType* gt = zoneData->gridType();

	m_solutions = vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(pd);
	ComboBoxTool::setupItems(gt->solutionCaptions(m_solutions), ui->solutionComboBox);

	m_scalars = vtkDataSetAttributesTool::getArrayNamesWithOneComponent(pd);
	ComboBoxTool::setupItems(gt->solutionCaptions(m_scalars), ui->scalarComboBox);

	if (m_solutions.size() < 2) {
		ui->physValLabel->hide();
		ui->solutionComboBox->hide();
	}
}

void Post2dWindowArrowUnstructuredSettingDialog::showRegionDialog()
{
	Post2dGridRegionSelectDialog dialog(this);
	if (! m_activeAvailable) {
		dialog.disableActive();
	}
	dialog.hideCustom();
	dialog.setRegionMode(m_setting.regionMode);
	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}
	m_setting.regionMode = dialog.regionMode();
}
