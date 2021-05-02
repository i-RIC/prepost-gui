#include "ui_continuoussnapshottimestepsettingpage.h"

#include "continuoussnapshottimestepsettingpage.h"
#include "continuoussnapshotwizard.h"

#include <guibase/timeformat/timeformatutil.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/posttimesteps.h>

ContinuousSnapshotTimestepSettingPage::ContinuousSnapshotTimestepSettingPage(QWidget* parent) :
	QWizardPage(parent),
	ui(new Ui::ContinuousSnapshotTimestepSettingPage)
{
	ui->setupUi(this);

	m_wizard = dynamic_cast<ContinuousSnapshotWizard*>(parent);

	connect(ui->startSlider, SIGNAL(valueChanged(int)), this, SLOT(handleStartChange(int)));
	connect(ui->stopSlider, SIGNAL(valueChanged(int)), this, SLOT(handleStopChange(int)));
}

ContinuousSnapshotTimestepSettingPage::~ContinuousSnapshotTimestepSettingPage()
{
	delete ui;
}

void ContinuousSnapshotTimestepSettingPage::initializePage()
{
	const auto& timeSteps = m_wizard->projectMainFile()->postSolutionInfo()->timeSteps()->timesteps();

	const auto& s = m_wizard->setting();

	ui->startSlider->setMinimum(1);
	ui->startSlider->setMaximum(timeSteps.size());

	ui->stopSlider->setMinimum(1);
	ui->stopSlider->setMaximum(timeSteps.size());

	ui->samplingSpinBox->setMinimum(1);
	ui->samplingSpinBox->setMaximum(timeSteps.size());

	ui->startSlider->setValue(s.startTimeStep + 1);
	if (s.startTimeStep == -1) {ui->startSlider->setValue(1);}
	ui->stopSlider->setValue(s.stopTimeStep + 1);
	if (s.stopTimeStep == -1) {ui->stopSlider->setValue(ui->stopSlider->maximum());}
	ui->samplingSpinBox->setValue(s.samplingRate);

	updateStartLabel();
	updateStopLabel();
}

bool ContinuousSnapshotTimestepSettingPage::validatePage()
{
	auto s = m_wizard->setting();

	s.startTimeStep = ui->startSlider->value() - 1;
	s.stopTimeStep = ui->stopSlider->value() - 1;
	s.samplingRate = ui->samplingSpinBox->value();

	m_wizard->setSetting(s);
	return true;
}

void ContinuousSnapshotTimestepSettingPage::handleStartChange(int time)
{
	updateStartLabel();

	if (ui->stopSlider->value() < time) {
		ui->stopSlider->setValue(time);
	}
}

void ContinuousSnapshotTimestepSettingPage::handleStopChange(int time)
{
	updateStopLabel();

	if (ui->startSlider->value() > time) {
		ui->startSlider->setValue(time);
	}
}

void ContinuousSnapshotTimestepSettingPage::updateStartLabel()
{
	const auto& timeSteps = m_wizard->projectMainFile()->postSolutionInfo()->timeSteps()->timesteps();
	double time = timeSteps.at(ui->startSlider->value() - 1);
	updateTimeLabel(time, ui->startValueLabel);
}

void ContinuousSnapshotTimestepSettingPage::updateStopLabel()
{
	const auto& timeSteps = m_wizard->projectMainFile()->postSolutionInfo()->timeSteps()->timesteps();
	double time = timeSteps.at(ui->stopSlider->value() - 1);
	updateTimeLabel(time, ui->stopValueLabel);
}

void ContinuousSnapshotTimestepSettingPage::updateTimeLabel(double timeValue, QLabel* label)
{
	auto mainFile = m_wizard->projectMainFile();

	auto zt = mainFile->zeroDateTime();
	auto f = mainFile->timeFormat();
	auto cf = mainFile->customTimeFormat();

	label->setText(TimeFormatUtil::formattedString(zt, timeValue, f, cf));
}
