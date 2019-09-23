#include "ui_graph2dverificationsettingdialog.h"

#include "graph2dverificationsettingdialog.h"

#include <guibase/comboboxtool.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/posttimesteps.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/project/measured/measureddata.h>

#include <QMessageBox>

#include <vtkDoubleArray.h>
#include <vtkPointData.h>

Graph2dVerificationSettingDialog::Graph2dVerificationSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Graph2dVerificationSettingDialog)
{
	ui->setupUi(this);
	connect(ui->fileComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(selectFile(int)));
}

Graph2dVerificationSettingDialog::~Graph2dVerificationSettingDialog()
{
	delete ui;
}

void Graph2dVerificationSettingDialog::setSetting(const Graph2dVerificationWindowResultSetting& setting)
{
	m_setting = setting;

	setPostSolutionInfo(m_setting.postSolutionInfo());
	//this->setMeasuredDatas(m_setting.m_measuredData);
	setMeasuredDatas(m_setting.measuredData());

	//this->setTimeStep(m_setting.m_timeStep);
	setTimeStep(m_setting.timeStep());

	//this->setPostZoneDataContainer(m_setting.m_activePostData);
	setPostZoneDataContainer(m_setting.activePostData());

	//this->setActiveResult(m_setting.m_activeResult);
	setActiveResult(m_setting.activeResult());

	//this->setMeasuredData(m_setting.m_activeMeasuredData);

	setActiveFile(m_setting.activeFile());

	setActiveValue(m_setting.activeValue());
}

const Graph2dVerificationWindowResultSetting& Graph2dVerificationSettingDialog::setting() const
{
	m_setting.setActivePostData(postZoneDataContainer());
	m_setting.setTimeStep(timeStep());							// int timestep
	m_setting.setActiveZone(activeZone());						// QString activeZone
	m_setting.setActiveResult(activeResult());					// QString activeResult
	m_setting.setActiveFile(activeFile());						// QString activeFile
	m_setting.setActiveValue(activeValue());					// QString activeValue
	m_setting.setActiveMeasuredData(measuredData());			//
	return m_setting;
}

void Graph2dVerificationSettingDialog::setPostSolutionInfo(PostSolutionInfo* info)
{
	m_postSolutionInfo = info;
	ui->gridComboBox->clear();
	for (PostZoneDataContainer* cont : info->zoneContainers2D()) {
		ui->gridComboBox->addItem(cont->zoneName().c_str());
	}
	selectZone(0);
	if (ui->gridComboBox->count() == 1) {
		///ui->gridLabel->hide();
		///ui->gridComboBox->hide();
	}

	PostTimeSteps* timesteps = m_postSolutionInfo->timeSteps();
	ui->timestepSlider->setValues(timesteps->timesteps());
	ui->timestepSlider->setValue(0);
}

void Graph2dVerificationSettingDialog::setMeasuredDatas(const std::vector<MeasuredData*>& data)
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

void Graph2dVerificationSettingDialog::setTimeStep(int step)
{
	ui->timestepSlider->setValue(step);
}

void Graph2dVerificationSettingDialog::setPostZoneDataContainer(PostZoneDataContainer* cont)
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

void Graph2dVerificationSettingDialog::setActiveResult(const QString& result)
{
	for (int i = 0; i < ui->physicalValueComboBox->count(); ++i) {
		QString item = ui->physicalValueComboBox->itemText(i);
		if (item == result) {
			ui->physicalValueComboBox->setCurrentIndex(i);
			return;
		}
	}
}

void Graph2dVerificationSettingDialog::setMeasuredData(MeasuredData* md)
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

void Graph2dVerificationSettingDialog::setActiveFile(const QString& file)
{
	for (int i = 0; i < ui->fileComboBox->count(); ++i) {
		QString item = ui->fileComboBox->itemText(i);
		if (item == file) {
			ui->fileComboBox->setCurrentIndex(i);
			return;
		}
	}
}


void Graph2dVerificationSettingDialog::setActiveValue(const QString& value)
{
	for (int i = 0; i < ui->measuredValueComboBox->count(); ++i) {
		QString item = ui->measuredValueComboBox->itemText(i);
		if (item == value) {
			ui->measuredValueComboBox->setCurrentIndex(i);
			return;
		}
	}
}

int Graph2dVerificationSettingDialog::timeStep() const
{
	return ui->timestepSlider->value();
}

PostZoneDataContainer* Graph2dVerificationSettingDialog::postZoneDataContainer() const
{
	int index = ui->gridComboBox->currentIndex();
	return m_postSolutionInfo->zoneContainers2D().at(index);
}


QString Graph2dVerificationSettingDialog::activeResult() const
{
	return ui->physicalValueComboBox->itemText(ui->physicalValueComboBox->currentIndex());
}

QString Graph2dVerificationSettingDialog::activeFile() const
{
	return ui->fileComboBox->itemText(ui->fileComboBox->currentIndex());
}

MeasuredData* Graph2dVerificationSettingDialog::measuredData() const
{
	int index = ui->fileComboBox->currentIndex();
	return m_measuredData.at(index);
}

QString Graph2dVerificationSettingDialog::activeValue() const
{
	return ui->measuredValueComboBox->itemText(ui->measuredValueComboBox->currentIndex());
}

QString Graph2dVerificationSettingDialog::activeZone() const
{
	return ui->gridComboBox->itemText(ui->gridComboBox->currentIndex());
}

void Graph2dVerificationSettingDialog::selectZone(int zoneid)
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

void Graph2dVerificationSettingDialog::selectFile(int fileid)
{
	MeasuredData* md = m_measuredData.at(fileid);
	ComboBoxTool::setupItems(md->scalarNames(), ui->measuredValueComboBox);
	ui->measuredValueComboBox->setCurrentIndex(0);
}

void Graph2dVerificationSettingDialog::accept()
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
