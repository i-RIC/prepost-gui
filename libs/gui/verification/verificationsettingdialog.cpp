#include "ui_verificationsettingdialog.h"

#include "verificationsettingdialog.h"

#include <guibase/comboboxtool.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/posttimesteps.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/project/measured/measureddata.h>

#include <QMessageBox>

#include <vtkDoubleArray.h>
#include <vtkPointData.h>

VerificationSettingDialog::VerificationSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::VerificationSettingDialog)
{
	ui->setupUi(this);
	connect(ui->fileComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(selectFile(int)));
}

VerificationSettingDialog::~VerificationSettingDialog()
{
	delete ui;
}

void VerificationSettingDialog::setPostSolutionInfo(PostSolutionInfo* info)
{
	m_postSolutionInfo = info;
	ui->gridComboBox->clear();
	for (PostZoneDataContainer* cont : info->zoneContainers2D()) {
		ui->gridComboBox->addItem(cont->zoneName().c_str());
	}
	selectZone(0);
	if (ui->gridComboBox->count() == 1) {
		ui->gridLabel->hide();
		ui->gridComboBox->hide();
	}

	PostTimeSteps* timesteps = m_postSolutionInfo->timeSteps();
	ui->timestepSlider->setValues(timesteps->timesteps());
	ui->timestepSlider->setValue(0);
}

void VerificationSettingDialog::setMeasuredDatas(const std::vector<MeasuredData*>& data)
{
	m_measuredData = data;
	ui->fileComboBox->clear();
	for (MeasuredData* md : data) {
		ui->fileComboBox->addItem(md->name());
	}
	selectFile(0);
	if (ui->fileComboBox->count() == 1) {
		ui->fileLabel->hide();
		ui->fileComboBox->hide();
	}
}

void VerificationSettingDialog::setTimeStep(int step)
{
	ui->timestepSlider->setValue(step);
}

void VerificationSettingDialog::setPostZoneDataContainer(PostZoneDataContainer* cont)
{
	for (int i = 0; i < m_postSolutionInfo->zoneContainers2D().count(); ++i) {
		PostZoneDataContainer* c = m_postSolutionInfo->zoneContainers2D().at(i);
		if (c == cont) {
			selectZone(i);
			return;
		}
	}
	selectZone(0);
}

void VerificationSettingDialog::setActiveResult(const QString& result)
{
	for (int i = 0; i < ui->physicalValueComboBox->count(); ++i) {
		QString item = ui->physicalValueComboBox->itemText(i);
		if (item == result) {
			ui->physicalValueComboBox->setCurrentIndex(i);
			return;
		}
	}
}

void VerificationSettingDialog::setMeasuredData(MeasuredData* md)
{
	for (int i = 0; i < m_measuredData.size(); ++i) {
		MeasuredData* d = m_measuredData.at(i);
		if (d == md) {
			selectFile(i);
			return;
		}
	}
	selectFile(0);
}

void VerificationSettingDialog::setActiveValue(const QString& value)
{
	for (int i = 0; i < ui->measuredValueComboBox->count(); ++i) {
		QString item = ui->measuredValueComboBox->itemText(i);
		if (item == value) {
			ui->measuredValueComboBox->setCurrentIndex(i);
			return;
		}
	}
}

int VerificationSettingDialog::timeStep()
{
	return ui->timestepSlider->value();
}

PostZoneDataContainer* VerificationSettingDialog::postZoneDataContainer()
{
	int index = ui->gridComboBox->currentIndex();
	return m_postSolutionInfo->zoneContainers2D().at(index);
}

QString VerificationSettingDialog::activeResult()
{
	return ui->physicalValueComboBox->itemText(ui->physicalValueComboBox->currentIndex());
}

MeasuredData* VerificationSettingDialog::measuredData()
{
	int index = ui->fileComboBox->currentIndex();
	return m_measuredData.at(index);
}

QString VerificationSettingDialog::activeValue()
{
	return ui->measuredValueComboBox->itemText(ui->measuredValueComboBox->currentIndex());
}

void VerificationSettingDialog::selectZone(int zoneid)
{
	ui->physicalValueComboBox->clear();
	PostZoneDataContainer* cont = m_postSolutionInfo->zoneContainers2D().at(zoneid);
	vtkPointData* pd = cont->data()->GetPointData();

	for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(pd)) {
		auto arr = pd->GetArray(name.c_str());
		if (vtkDoubleArray::SafeDownCast(arr) == nullptr) {continue;}

		ui->physicalValueComboBox->addItem(name.c_str());
	}
	ui->physicalValueComboBox->setCurrentIndex(0);
}

void VerificationSettingDialog::selectFile(int fileid)
{
	MeasuredData* md = m_measuredData.at(fileid);
	ComboBoxTool::setupItems(md->scalarNames(), ui->measuredValueComboBox);
	ui->measuredValueComboBox->setCurrentIndex(0);
}

void VerificationSettingDialog::accept()
{
	if (activeResult() == "") {
		QMessageBox::warning(this, tr("Warning"), tr("No Physical Value is selected."));
		return;
	}
	if (activeValue() == "") {
		QMessageBox::warning(this, tr("Warning"), tr("No Value is selected."));
		return;
	}
	QDialog::accept();
}
