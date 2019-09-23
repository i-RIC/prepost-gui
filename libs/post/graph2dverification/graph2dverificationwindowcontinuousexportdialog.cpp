#include "ui_graph2dverificationwindowcontinuousexportdialog.h"

#include "graph2dverificationwindowcontinuousexportdialog.h"

#include <QDir>
#include <QFile>
#include <QMessageBox>

Graph2dVerificationWindowContinuousExportDialog::Graph2dVerificationWindowContinuousExportDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Graph2dVerificationWindowContinuousExportDialog)
{
	ui->setupUi(this);
	connect(ui->currentRegionCheckBox, SIGNAL(toggled(bool)), this, SLOT(regionCurrentToggled(bool)));
	connect(ui->fullRegionCheckBox, SIGNAL(toggled(bool)), this, SLOT(fullRegionToggled(bool)));
	connect(ui->iMinSlider, SIGNAL(valueChanged(int)), this, SLOT(iMinChanged(int)));
	connect(ui->iMaxSlider, SIGNAL(valueChanged(int)), this, SLOT(iMaxChanged(int)));
	connect(ui->jMinSlider, SIGNAL(valueChanged(int)), this, SLOT(jMinChanged(int)));
	connect(ui->jMaxSlider, SIGNAL(valueChanged(int)), this, SLOT(jMaxChanged(int)));
	connect(ui->kMinSlider, SIGNAL(valueChanged(int)), this, SLOT(kMinChanged(int)));
	connect(ui->kMaxSlider, SIGNAL(valueChanged(int)), this, SLOT(kMaxChanged(int)));
	connect(ui->indexMinSlider, SIGNAL(valueChanged(int)), this, SLOT(indexMinChanged(int)));
	connect(ui->indexMaxSlider, SIGNAL(valueChanged(int)), this, SLOT(indexMaxChanged(int)));

	connect(ui->timeCurrentOnlyCheckBox, SIGNAL(toggled(bool)), this, SLOT(timeCurrentToggled(bool)));
	connect(ui->timeAllCheckBox, SIGNAL(toggled(bool)), this, SLOT(timeAllToggled(bool)));
	connect(ui->timeStartSlider, SIGNAL(valueChanged(int)), this, SLOT(timeStartChanged(int)));
	connect(ui->timeEndSlider, SIGNAL(valueChanged(int)), this, SLOT(timeEndChanged(int)));
}

Graph2dVerificationWindowContinuousExportDialog::~Graph2dVerificationWindowContinuousExportDialog()
{
	delete ui;
}

void Graph2dVerificationWindowContinuousExportDialog::setSetting(const Graph2dVerificationWindowResultSetting& setting, int dim[4])
{
#if 0 || 1
	Graph2dVerificationWindowResultSetting::DataTypeInfo* info = setting.targetDataTypeInfo();
#endif
	ui->iMinSlider->setRange(1, dim[0]);
	ui->iMaxSlider->setRange(1, dim[0]);
	ui->jMinSlider->setRange(1, dim[1]);
	ui->jMaxSlider->setRange(1, dim[1]);
	ui->kMinSlider->setRange(1, dim[2]);
	ui->kMaxSlider->setRange(1, dim[2]);
	ui->indexMinSlider->setRange(1, dim[3]);
	ui->indexMaxSlider->setRange(1, dim[3]);

	m_i = setting.gridI();
	m_j = setting.gridJ();
	m_k = setting.gridK();
	m_index = setting.gridIndex();

	ui->iMinLabel->hide();
	ui->iMinSlider->hide();
	ui->iMaxLabel->hide();
	ui->iMaxSlider->hide();
	ui->jMinLabel->hide();
	ui->jMinSlider->hide();
	ui->jMaxLabel->hide();
	ui->jMaxSlider->hide();
	ui->kMinLabel->hide();
	ui->kMinSlider->hide();
	ui->kMaxLabel->hide();
	ui->kMaxSlider->hide();
	ui->indexMinLabel->hide();
	ui->indexMinSlider->hide();
	ui->indexMaxLabel->hide();
	ui->indexMaxSlider->hide();

#if 0 || 1
	if (info == nullptr) {return;}
	switch (info->dataType) {
	case Graph2dVerificationWindowResultSetting::dtBaseIterative:
		break;
	case Graph2dVerificationWindowResultSetting::dtDim1DStructured:
		if (setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaTime) {
			ui->iMinLabel->show();
			ui->iMinSlider->show();
			ui->iMaxLabel->show();
			ui->iMaxSlider->show();
		}
		break;
	case Graph2dVerificationWindowResultSetting::dtDim2DStructured:
		if (setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaTime) {
			ui->iMinLabel->show();
			ui->iMinSlider->show();
			ui->iMaxLabel->show();
			ui->iMaxSlider->show();
			ui->jMinLabel->show();
			ui->jMinSlider->show();
			ui->jMaxLabel->show();
			ui->jMaxSlider->show();
		} else if (setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaI) {
			ui->jMinLabel->show();
			ui->jMinSlider->show();
			ui->jMaxLabel->show();
			ui->jMaxSlider->show();
		} else if (setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaJ) {
			ui->iMinLabel->show();
			ui->iMinSlider->show();
			ui->iMaxLabel->show();
			ui->iMaxSlider->show();
		}
		break;
	case Graph2dVerificationWindowResultSetting::dtDim3DStructured:
		if (setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaTime) {
			ui->iMinLabel->show();
			ui->iMinSlider->show();
			ui->iMaxLabel->show();
			ui->iMaxSlider->show();
			ui->jMinLabel->show();
			ui->jMinSlider->show();
			ui->jMaxLabel->show();
			ui->jMaxSlider->show();
			ui->kMinLabel->show();
			ui->kMinSlider->show();
			ui->kMaxLabel->show();
			ui->kMaxSlider->show();
		} else if (setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaI) {
			ui->jMinLabel->show();
			ui->jMinSlider->show();
			ui->jMaxLabel->show();
			ui->jMaxSlider->show();
			ui->kMinLabel->show();
			ui->kMinSlider->show();
			ui->kMaxLabel->show();
			ui->kMaxSlider->show();
		} else if (setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaJ) {
			ui->iMinLabel->show();
			ui->iMinSlider->show();
			ui->iMaxLabel->show();
			ui->iMaxSlider->show();
			ui->kMinLabel->show();
			ui->kMinSlider->show();
			ui->kMaxLabel->show();
			ui->kMaxSlider->show();
		} else if (setting.xAxisMode() == Graph2dVerificationWindowResultSetting::xaK) {
			ui->iMinLabel->show();
			ui->iMinSlider->show();
			ui->iMaxLabel->show();
			ui->iMaxSlider->show();
			ui->jMinLabel->show();
			ui->jMinSlider->show();
			ui->jMaxLabel->show();
			ui->jMaxSlider->show();
		}
		break;
	case Graph2dVerificationWindowResultSetting::dtDim1DUnstructured:
	case Graph2dVerificationWindowResultSetting::dtDim2DUnstructured:
	case Graph2dVerificationWindowResultSetting::dtDim3DUnstructured:
		ui->indexMinLabel->show();
		ui->indexMinSlider->show();
		ui->indexMaxLabel->show();
		ui->indexMaxSlider->show();
		break;
	}
#endif
	adjustSize();
}

void Graph2dVerificationWindowContinuousExportDialog::setTimesteps(QList<double> steps)
{
	ui->timeStartSlider->setValues(steps);
	ui->timeEndSlider->setValues(steps);
}

void Graph2dVerificationWindowContinuousExportDialog::setCurrentStep(int step)
{
	m_currentStep = step;
}

void Graph2dVerificationWindowContinuousExportDialog::setFolder(const QString& folder)
{
	ui->directoryEditWidget->setDirname(folder);
}

void Graph2dVerificationWindowContinuousExportDialog::setPrefix(const QString& prefix)
{
	ui->prefixEdit->setText(prefix);
}

void Graph2dVerificationWindowContinuousExportDialog::setRegionMode(RegionMode rm)
{
	if (rm == rmCurrentOnly) {
		ui->currentRegionCheckBox->setChecked(true);
		regionCurrentToggled(true);
	} else if (rm == rmAll) {
		fullRegionToggled(true);
		ui->fullRegionCheckBox->setChecked(true);
	} else {
		ui->currentRegionCheckBox->setChecked(false);
		ui->fullRegionCheckBox->setChecked(false);
	}
}

void Graph2dVerificationWindowContinuousExportDialog::setIMin(int imin)
{
	if (regionMode() != rmCustom) {return;}
	ui->iMinSlider->setValue(imin + 1);
}

void Graph2dVerificationWindowContinuousExportDialog::setIMax(int imax)
{
	if (regionMode() != rmCustom) {return;}
	ui->iMaxSlider->setValue(imax + 1);
}

void Graph2dVerificationWindowContinuousExportDialog::setJMin(int jmin)
{
	if (regionMode() != rmCustom) {return;}
	ui->jMinSlider->setValue(jmin + 1);
}

void Graph2dVerificationWindowContinuousExportDialog::setJMax(int jmax)
{
	if (regionMode() != rmCustom) {return;}
	ui->jMaxSlider->setValue(jmax + 1);
}

void Graph2dVerificationWindowContinuousExportDialog::setKMin(int kmin)
{
	if (regionMode() != rmCustom) {return;}
	ui->kMinSlider->setValue(kmin + 1);
}

void Graph2dVerificationWindowContinuousExportDialog::setKMax(int kmax)
{
	if (regionMode() != rmCustom) {return;}
	ui->kMaxSlider->setValue(kmax + 1);
}

void Graph2dVerificationWindowContinuousExportDialog::setIndexMin(int idxmin)
{
	if (regionMode() != rmCustom) {return;}
	ui->indexMinSlider->setValue(idxmin + 1);
}

void Graph2dVerificationWindowContinuousExportDialog::setIndexMax(int idxmax)
{
	if (regionMode() != rmCustom) {return;}
	ui->indexMaxSlider->setValue(idxmax + 1);
}

void Graph2dVerificationWindowContinuousExportDialog::setTimeMode(TimeMode tm)
{
	if (tm == tmCurrentOnly) {
		ui->timeCurrentOnlyCheckBox->setChecked(true);
		ui->timeStartSlider->setValue(m_currentStep);
		ui->timeEndSlider->setValue(m_currentStep);
	} else if (tm == tmAll) {
		ui->timeAllCheckBox->setChecked(true);
		ui->timeStartSlider->setValue(ui->timeStartSlider->minimum());
		ui->timeEndSlider->setValue(ui->timeEndSlider->maximum());
	} else if (tm == tmCustom) {
		ui->timeCurrentOnlyCheckBox->setChecked(false);
		ui->timeAllCheckBox->setChecked(false);
	}
}

void Graph2dVerificationWindowContinuousExportDialog::setStartTimeStep(int startstep)
{
	if (ui->timeCurrentOnlyCheckBox->isChecked()) {return;}
	if (ui->timeAllCheckBox->isChecked()) {return;}
	ui->timeStartSlider->setValue(startstep);
}

void Graph2dVerificationWindowContinuousExportDialog::setEndTimeStep(int endstep)
{
	if (ui->timeCurrentOnlyCheckBox->isChecked()) {return;}
	if (ui->timeAllCheckBox->isChecked()) {return;}
	ui->timeEndSlider->setValue(endstep);
}

void Graph2dVerificationWindowContinuousExportDialog::setTimeSkip(int skip)
{
	if (ui->timeCurrentOnlyCheckBox->isChecked()) {return;}
	if (ui->timeAllCheckBox->isChecked()) {return;}
	ui->samplingSpinBox->setValue(skip);
}

QString Graph2dVerificationWindowContinuousExportDialog::folder() const
{
	return ui->directoryEditWidget->dirname();
}

QString Graph2dVerificationWindowContinuousExportDialog::prefix() const
{
	return ui->prefixEdit->text();
}

Graph2dVerificationWindowContinuousExportDialog::RegionMode Graph2dVerificationWindowContinuousExportDialog::regionMode() const
{
	if (ui->currentRegionCheckBox->isChecked()) {
		return rmCurrentOnly;
	} else if (ui->fullRegionCheckBox->isChecked()) {
		return rmAll;
	} else {
		return rmCustom;
	}
}

int Graph2dVerificationWindowContinuousExportDialog::iMin() const
{
	return ui->iMinSlider->value() - 1;
}

int Graph2dVerificationWindowContinuousExportDialog::iMax() const
{
	return ui->iMaxSlider->value() - 1;
}

int Graph2dVerificationWindowContinuousExportDialog::jMin() const
{
	return ui->jMinSlider->value() - 1;
}

int Graph2dVerificationWindowContinuousExportDialog::jMax() const
{
	return ui->jMaxSlider->value() - 1;
}

int Graph2dVerificationWindowContinuousExportDialog::kMin() const
{
	return ui->kMinSlider->value() - 1;
}

int Graph2dVerificationWindowContinuousExportDialog::kMax() const
{
	return ui->kMaxSlider->value() - 1;
}
int Graph2dVerificationWindowContinuousExportDialog::indexMin() const
{
	return ui->indexMinSlider->value() - 1;
}

int Graph2dVerificationWindowContinuousExportDialog::indexMax() const
{
	return ui->indexMaxSlider->value() - 1;
}

Graph2dVerificationWindowContinuousExportDialog::TimeMode Graph2dVerificationWindowContinuousExportDialog::timeMode() const
{
	if (ui->timeCurrentOnlyCheckBox->isChecked()) {
		return tmCurrentOnly;
	} else if (ui->timeAllCheckBox->isChecked()) {
		return tmAll;
	} else {
		return tmCustom;
	}
}

int Graph2dVerificationWindowContinuousExportDialog::startTimeStep() const
{
	return ui->timeStartSlider->value();
}

int Graph2dVerificationWindowContinuousExportDialog::endTimeStep() const
{
	return ui->timeEndSlider->value();
}

int Graph2dVerificationWindowContinuousExportDialog::timeSkip() const
{
	return ui->samplingSpinBox->value();
}


void Graph2dVerificationWindowContinuousExportDialog::accept()
{
	if (! QFile::exists(ui->directoryEditWidget->dirname())) {
		QMessageBox::warning(this, tr("Warning"), tr("Folder %1 does not exists."), QDir::toNativeSeparators(ui->directoryEditWidget->dirname()));
		return;
	}
	if (ui->prefixEdit->text() == "") {
		QMessageBox::warning(this, tr("Warning"), tr("Please input prefix."));
		return;
	}
	QDialog::accept();
}

void Graph2dVerificationWindowContinuousExportDialog::regionCurrentToggled(bool toggled)
{
	ui->fullRegionCheckBox->setEnabled(! toggled);
	if (! toggled) {return;}
	ui->fullRegionCheckBox->setChecked(false);
	ui->iMinSlider->setValue(m_i + 1);
	ui->iMaxSlider->setValue(m_i + 1);
	ui->jMinSlider->setValue(m_j + 1);
	ui->jMaxSlider->setValue(m_j + 1);
	ui->kMinSlider->setValue(m_k + 1);
	ui->kMaxSlider->setValue(m_k + 1);
	ui->indexMinSlider->setValue(m_index + 1);
	ui->indexMaxSlider->setValue(m_index + 1);
}

void Graph2dVerificationWindowContinuousExportDialog::fullRegionToggled(bool toggled)
{
	ui->currentRegionCheckBox->setEnabled(! toggled);
	if (! toggled) {return;}
	ui->currentRegionCheckBox->setChecked(false);
	ui->iMinSlider->setValue(ui->iMinSlider->minimum());
	ui->iMaxSlider->setValue(ui->iMaxSlider->maximum());
	ui->jMinSlider->setValue(ui->jMinSlider->minimum());
	ui->jMaxSlider->setValue(ui->jMaxSlider->maximum());
	ui->kMinSlider->setValue(ui->kMinSlider->minimum());
	ui->kMaxSlider->setValue(ui->kMaxSlider->maximum());
	ui->indexMinSlider->setValue(ui->indexMinSlider->minimum());
	ui->indexMaxSlider->setValue(ui->indexMaxSlider->maximum());
}

void Graph2dVerificationWindowContinuousExportDialog::iMinChanged(int imin)
{
	if (ui->iMaxSlider->value() < imin) {
		ui->iMaxSlider->setValue(imin);
	}
}

void Graph2dVerificationWindowContinuousExportDialog::iMaxChanged(int imax)
{
	if (ui->iMinSlider->value() > imax) {
		ui->iMinSlider->setValue(imax);
	}
}

void Graph2dVerificationWindowContinuousExportDialog::jMinChanged(int jmin)
{
	if (ui->jMaxSlider->value() < jmin) {
		ui->jMaxSlider->setValue(jmin);
	}
}

void Graph2dVerificationWindowContinuousExportDialog::jMaxChanged(int jmax)
{
	if (ui->jMinSlider->value() > jmax) {
		ui->jMinSlider->setValue(jmax);
	}
}

void Graph2dVerificationWindowContinuousExportDialog::kMinChanged(int kmin)
{
	if (ui->kMaxSlider->value() < kmin) {
		ui->kMaxSlider->setValue(kmin);
	}
}

void Graph2dVerificationWindowContinuousExportDialog::kMaxChanged(int kmax)
{
	if (ui->kMinSlider->value() > kmax) {
		ui->kMinSlider->setValue(kmax);
	}
}

void Graph2dVerificationWindowContinuousExportDialog::indexMinChanged(int imin)
{
	if (ui->indexMaxSlider->value() < imin) {
		ui->indexMaxSlider->setValue(imin);
	}
}

void Graph2dVerificationWindowContinuousExportDialog::indexMaxChanged(int imax)
{
	if (ui->indexMinSlider->value() > imax) {
		ui->indexMinSlider->setValue(imax);
	}
}

void Graph2dVerificationWindowContinuousExportDialog::timeCurrentToggled(bool toggled)
{
	ui->timeAllCheckBox->setEnabled(! toggled);
	if (toggled) {
		ui->timeAllCheckBox->setChecked(false);
		ui->timeStartSlider->setValue(m_currentStep);
		ui->timeEndSlider->setValue(m_currentStep);
	}
}

void Graph2dVerificationWindowContinuousExportDialog::timeAllToggled(bool toggled)
{
	ui->timeCurrentOnlyCheckBox->setEnabled(! toggled);
	if (toggled) {
		ui->timeCurrentOnlyCheckBox->setChecked(false);
		ui->timeStartSlider->setValue(ui->timeStartSlider->minimum());
		ui->timeEndSlider->setValue(ui->timeEndSlider->maximum());
	}
}

void Graph2dVerificationWindowContinuousExportDialog::timeStartChanged(int start)
{
	if (ui->timeEndSlider->value() < start) {
		ui->timeEndSlider->setValue(start);
	}
}

void Graph2dVerificationWindowContinuousExportDialog::timeEndChanged(int end)
{
	if (ui->timeStartSlider->value() > end) {
		ui->timeStartSlider->setValue(end);
	}
}
