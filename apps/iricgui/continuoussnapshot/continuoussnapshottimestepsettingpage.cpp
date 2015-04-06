#include "continuoussnapshottimestepsettingpage.h"
#include "continuoussnapshotwizard.h"
#include "ui_continuoussnapshottimestepsettingpage.h"

ContinuousSnapshotTimestepSettingPage::ContinuousSnapshotTimestepSettingPage(QWidget *parent) :
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
	ui->startSlider->setValues(m_wizard->timeSteps());
	ui->stopSlider->setValues(m_wizard->timeSteps());
	ui->samplingSpinBox->setMaximum(m_wizard->timeSteps().size());

	ui->startSlider->setValue(m_wizard->start());
	if (m_wizard->start() == -1){ui->startSlider->setValue(0);}
	ui->stopSlider->setValue(m_wizard->stop());
	if (m_wizard->stop() == -1){ui->stopSlider->setValue(ui->stopSlider->maximum());}
	ui->samplingSpinBox->setValue(m_wizard->samplingRate());
}

bool ContinuousSnapshotTimestepSettingPage::validatePage()
{
	m_wizard->setStart(ui->startSlider->value());
	m_wizard->setStop(ui->stopSlider->value());
	m_wizard->setSamplingRate(ui->samplingSpinBox->value());
	return true;
}

void ContinuousSnapshotTimestepSettingPage::modifyStart(int time)
{
	if (ui->startSlider->value() > time){
		ui->startSlider->setValue(time);
	}
}

void ContinuousSnapshotTimestepSettingPage::modifyStop(int time)
{
	if (ui->stopSlider->value() < time){
		ui->stopSlider->setValue(time);
	}
}
