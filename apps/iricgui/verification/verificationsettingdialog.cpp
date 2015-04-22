#include "ui_verificationsettingdialog.h"
#include "verificationsettingdialog.h"

#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/posttimesteps.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/project/measureddata.h>

#include <QMessageBox>

#include <vtkPointData.h>

VerificationSettingDialog::VerificationSettingDialog(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::VerificationSettingDialog)
{
	ui->setupUi(this);
}

VerificationSettingDialog::~VerificationSettingDialog()
{
	delete ui;
}

void VerificationSettingDialog::setPostSolutionInfo(PostSolutionInfo* info)
{
	m_postSolutionInfo = info;
	ui->gridComboBox->clear();
	const QList<PostZoneDataContainer*>& containers = info->zoneContainers2D();
	for (int i = 0; i < containers.size(); ++i){
		PostZoneDataContainer* cont = containers.at(i);
		ui->gridComboBox->addItem(cont->zoneName());
	}
	selectZone(0);
	if (ui->gridComboBox->count() == 1){
		ui->gridLabel->hide();
		ui->gridComboBox->hide();
	}

	PostTimeSteps* timesteps = m_postSolutionInfo->timeSteps();
	ui->timestepSlider->setValues(timesteps->timesteps());
	ui->timestepSlider->setValue(0);
}

void VerificationSettingDialog::setMeasuredDatas(const QList<MeasuredData*>& data)
{
	m_measuredData = data;
	ui->fileComboBox->clear();
	for (int i = 0; i < data.size(); ++i){
		MeasuredData* md = data.at(i);
		ui->fileComboBox->addItem(md->name());
	}
	selectFile(0);
	if (ui->fileComboBox->count() == 1){
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
	for (int i = 0; i < m_postSolutionInfo->zoneContainers2D().count(); ++i){
		PostZoneDataContainer* c = m_postSolutionInfo->zoneContainers2D().at(i);
		if (c == cont){
			selectZone(i);
			return;
		}
	}
	selectZone(0);
}

void VerificationSettingDialog::setActiveResult(const QString& result)
{
	for (int i = 0; i < ui->physicalValueComboBox->count(); ++i){
		QString item = ui->physicalValueComboBox->itemText(i);
		if (item == result){
			ui->physicalValueComboBox->setCurrentIndex(i);
			return;
		}
	}
}

void VerificationSettingDialog::setMeasuredData(MeasuredData* md)
{
	for (int i = 0; i < m_measuredData.count(); ++i){
		MeasuredData* d = m_measuredData.at(i);
		if (d == md){
			selectFile(i);
			return;
		}
	}
	selectFile(0);
}

void VerificationSettingDialog::setActiveValue(const QString& value)
{
	for (int i = 0; i < ui->measuredValueComboBox->count(); ++i){
		QString item = ui->measuredValueComboBox->itemText(i);
		if (item == value){
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
	vtkPointSet* ps = cont->data();
	vtkPointData* pd = ps->GetPointData();

	int number = pd->GetNumberOfArrays();
	for (int i = 0; i < number; i++){
		vtkAbstractArray* tmparray = pd->GetArray(i);
		if (tmparray == nullptr){
			continue;
		}
		if (tmparray->GetNumberOfComponents() > 1){
			// vector attribute.
			continue;
		}
		QString name = pd->GetArray(i)->GetName();
		ui->physicalValueComboBox->addItem(name);
	}
	ui->physicalValueComboBox->setCurrentIndex(0);
}

void VerificationSettingDialog::selectFile(int fileid)
{
	ui->measuredValueComboBox->clear();
	MeasuredData* md = m_measuredData.at(fileid);
	for (int i = 0; i < md->pointNames().count(); ++i){
		ui->measuredValueComboBox->addItem(md->pointNames().at(i));
	}
	ui->measuredValueComboBox->setCurrentIndex(0);
}

void VerificationSettingDialog::accept()
{
	if (activeResult() == ""){
		QMessageBox::warning(this, tr("Warning"), tr("No Physical Value is selected."));
		return;
	}
	if (activeValue() == ""){
		QMessageBox::warning(this, tr("Warning"), tr("No Value is selected."));
		return;
	}
	QDialog::accept();
}
