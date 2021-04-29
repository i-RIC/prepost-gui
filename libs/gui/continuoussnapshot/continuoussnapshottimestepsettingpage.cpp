#include "ui_continuoussnapshottimestepsettingpage.h"

#include "continuoussnapshottimestepsettingpage.h"
#include "continuoussnapshotwizard.h"

ContinuousSnapshotTimestepSettingPage::ContinuousSnapshotTimestepSettingPage(QWidget* parent) :
	QWizardPage(parent),
	ui(new Ui::ContinuousSnapshotTimestepSettingPage)
{
	ui->setupUi(this);

	m_wizard = dynamic_cast<ContinuousSnapshotWizard*>(parent);

	connect(ui->startSlider, SIGNAL(valueChanged(int)), this, SLOT(modifyStop(int)));
	connect(ui->stopSlider, SIGNAL(valueChanged(int)), this, SLOT(modifyStart(int)));
}

ContinuousSnapshotTimestepSettingPage::~ContinuousSnapshotTimestepSettingPage()
{
	delete ui;
}

void ContinuousSnapshotTimestepSettingPage::initializePage()
{
	const auto& s = m_wizard->setting();
	ui->startSlider->setValues(s.timeSteps);
	ui->stopSlider->setValues(s.timeSteps);
	ui->samplingSpinBox->setMaximum(s.timeSteps.size());

	ui->startSlider->setValue(s.startTimeStep);
	if (s.startTimeStep == -1) {ui->startSlider->setValue(0);}
	ui->stopSlider->setValue(s.stopTimeStep);
	if (s.stopTimeStep == -1) {ui->stopSlider->setValue(ui->stopSlider->maximum());}
	ui->samplingSpinBox->setValue(s.samplingRate);
}

bool ContinuousSnapshotTimestepSettingPage::validatePage()
{
	auto s = m_wizard->setting();

	s.startTimeStep = ui->startSlider->value();
	s.stopTimeStep = ui->stopSlider->value();
	s.samplingRate = ui->samplingSpinBox->value();

	m_wizard->setSetting(s);
	return true;
}

void ContinuousSnapshotTimestepSettingPage::modifyStart(int time)
{
	if (ui->startSlider->value() > time) {
		ui->startSlider->setValue(time);
	}
}

void ContinuousSnapshotTimestepSettingPage::modifyStop(int time)
{
	if (ui->stopSlider->value() < time) {
		ui->stopSlider->setValue(time);
	}
}
