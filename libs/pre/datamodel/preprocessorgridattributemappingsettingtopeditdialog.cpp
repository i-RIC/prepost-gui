#include "ui_preprocessorgridattributemappingsettingtopeditdialog.h"

#include "preprocessorgridattributemappingsettingtopeditdialog.h"
#include "preprocessorgridattributetemplatemappingsettingdialog.h"

PreProcessorGridAttributeMappingSettingTopEditDialog::PreProcessorGridAttributeMappingSettingTopEditDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::PreProcessorGridAttributeMappingSettingTopEditDialog)
{
	ui->setupUi(this);
	connect(ui->templateDetailButton, SIGNAL(clicked()), this, SLOT(showDetailDialog()));
}

PreProcessorGridAttributeMappingSettingTopEditDialog::~PreProcessorGridAttributeMappingSettingTopEditDialog()
{
	delete ui;
}

PreProcessorGridAttributeMappingMode::Mode PreProcessorGridAttributeMappingSettingTopEditDialog::mappingMode()
{
	if (ui->autoRadioButton->isChecked()) {
		return PreProcessorGridAttributeMappingMode::mAuto;
	} else {
		return PreProcessorGridAttributeMappingMode::mManual;
	}

}

void PreProcessorGridAttributeMappingSettingTopEditDialog::setMappingMode(PreProcessorGridAttributeMappingMode::Mode mm)
{
	if (mm == PreProcessorGridAttributeMappingMode::mAuto) {
		ui->autoRadioButton->setChecked(true);
	} else {
		ui->manualRadioButton->setChecked(true);
	}
}

RawDataPointmapMappingMode::Mode PreProcessorGridAttributeMappingSettingTopEditDialog::pointmapMappingMode()
{
	if (ui->TINRadioButton->isChecked()) {
		return RawDataPointmapMappingMode::mTIN;
	} else {
		return RawDataPointmapMappingMode::mTemplate;
	}
}

void PreProcessorGridAttributeMappingSettingTopEditDialog::setPointmapMappingMode(RawDataPointmapMappingMode::Mode mm)
{
	if (mm == RawDataPointmapMappingMode::mTIN) {
		ui->TINRadioButton->setChecked(true);
	} else {
		ui->templateRadioButton->setChecked(true);
	}
}

void PreProcessorGridAttributeMappingSettingTopEditDialog::showDetailDialog()
{
	PreProcessorGridAttributeTemplateMappingSettingDialog dialog;
	dialog.setAutoMode(m_autoMode);
	dialog.setStreamWiseLength(m_streamWiseLength);
	dialog.setCrossStreamLength(m_crossStreamLength);
	dialog.setNumExpansion(m_numExpansion);
	dialog.setWeightExponent(m_weightExponent);

	int result = dialog.exec();
	if (result == QDialog::Rejected) {return;}

	m_autoMode = dialog.autoMode();
	m_streamWiseLength = dialog.streamWiseLength();
	m_crossStreamLength = dialog.crossStreamLength();
	m_numExpansion = dialog.numExpansion();
	m_weightExponent = dialog.weightExponent();
}
