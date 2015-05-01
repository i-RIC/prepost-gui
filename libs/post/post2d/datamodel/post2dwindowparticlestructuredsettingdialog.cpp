#include "../post2dgridregionselectdialog.h"
#include "post2dwindowparticlestructuredsettingdialog.h"
#include "ui_post2dwindowparticlestructuredsettingdialog.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>

#include <vtkPointData.h>
#include <vtkStructuredGrid.h>

Post2dWindowParticleStructuredSettingDialog::Post2dWindowParticleStructuredSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Post2dWindowParticleStructuredSettingDialog)
{
	ui->setupUi(this);

	m_timeMode = Post2dWindowNodeVectorParticleGroupDataItem::tmNormal;
	m_applying = false;

	setupNominations();
	ui->timeSlider->setTracking(true);
	ui->spaceSlider->setTracking(true);

	connect(ui->startPositionListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(activeDataChanged(int)));
	connect(ui->iminSlider, SIGNAL(valueChanged(int)), this, SLOT(iMinChanged(int)));
	connect(ui->imaxSlider, SIGNAL(valueChanged(int)), this, SLOT(iMaxChanged(int)));
	connect(ui->jminSlider, SIGNAL(valueChanged(int)), this, SLOT(jMinChanged(int)));
	connect(ui->jmaxSlider, SIGNAL(valueChanged(int)), this, SLOT(jMaxChanged(int)));
	connect(ui->timeSlider, SIGNAL(valueChanged(int)), this, SLOT(handleTimeSliderChange(int)));
	connect(ui->spaceSlider, SIGNAL(valueChanged(int)), this, SLOT(handleSpaceSliderChange(int)));
	connect(ui->colorWidget, SIGNAL(colorChanged(QColor)), this, SLOT(colorChanged(QColor)));
	connect(ui->sizeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(sizeChanged(int)));

	connect(ui->addPushButton, SIGNAL(clicked()), this, SLOT(addData()));
	connect(ui->removePushButton, SIGNAL(clicked()), this, SLOT(removeData()));
	connect(ui->regionSettingButton, SIGNAL(clicked()), this, SLOT(showRegionDialog()));
}

Post2dWindowParticleStructuredSettingDialog::~Post2dWindowParticleStructuredSettingDialog()
{
	delete ui;
}

void Post2dWindowParticleStructuredSettingDialog::setZoneData(PostZoneDataContainer* zoneData)
{
	vtkStructuredGrid* g = dynamic_cast<vtkStructuredGrid*>(zoneData->data());
	int dim[3];
	g->GetDimensions(dim);
	ui->iminSlider->setRange(1, dim[0]);
	ui->imaxSlider->setRange(1, dim[0]);
	ui->jminSlider->setRange(1, dim[1]);
	ui->jmaxSlider->setRange(1, dim[1]);
	setupSolutionComboBox(zoneData);
}

void Post2dWindowParticleStructuredSettingDialog::setSolution(const QString& sol)
{
	int index = m_solutions.indexOf(sol);
	if (index == -1) {index = 0;}
	ui->solutionComboBox->setCurrentIndex(index);
}

const QString Post2dWindowParticleStructuredSettingDialog::solution()
{
	int index = ui->solutionComboBox->currentIndex();
	return m_solutions.at(index);
}

void Post2dWindowParticleStructuredSettingDialog::setTimeMode(Post2dWindowNodeVectorParticleGroupDataItem::TimeMode tm)
{
	m_timeMode = tm;
	if (m_timeMode == Post2dWindowNodeVectorParticleGroupDataItem::tmNormal) {
		ui->timeSlider->setValue(m_skipNominations.count());
	}
}

Post2dWindowNodeVectorParticleGroupDataItem::TimeMode Post2dWindowParticleStructuredSettingDialog::timeMode()
{
	if (ui->timeSlider->value() == m_skipNominations.count()) {
		return Post2dWindowNodeVectorParticleGroupDataItem::tmNormal;
	} else if (ui->timeSlider->value() < m_skipNominations.count()) {
		return Post2dWindowNodeVectorParticleGroupDataItem::tmSkip;
	} else {
		return Post2dWindowNodeVectorParticleGroupDataItem::tmSubdivide;
	}
}

void Post2dWindowParticleStructuredSettingDialog::setTimeSamplingRate(int sr)
{
	if (m_timeMode != Post2dWindowNodeVectorParticleGroupDataItem::tmSkip) {return;}
	for (int i = 0; i < m_skipNominations.count(); ++i) {
		if (m_skipNominations.at(i) == sr) {
			ui->timeSlider->setValue(m_skipNominations.count() - i - 1);
			return;
		}
	}
}

int Post2dWindowParticleStructuredSettingDialog::timeSamplingRate()
{
	if (ui->timeSlider->value() >= m_skipNominations.count()) {return 1;}
	return m_skipNominations.at(m_skipNominations.count() - ui->timeSlider->value() - 1);
}

void Post2dWindowParticleStructuredSettingDialog::setTimeDivision(int sd)
{
	if (m_timeMode != Post2dWindowNodeVectorParticleGroupDataItem::tmSubdivide) {return;}
	for (int i = 0; i < m_subDivNominations.count(); ++i) {
		if (m_subDivNominations.at(i) == sd) {
			ui->timeSlider->setValue(m_skipNominations.count() + i + 1);
			return;
		}
	}
}

int Post2dWindowParticleStructuredSettingDialog::timeDivision()
{
	if (ui->timeSlider->value() <= m_skipNominations.count()) {return 1;}
	return m_subDivNominations.at(ui->timeSlider->value() - m_skipNominations.count() - 1);
}

void Post2dWindowParticleStructuredSettingDialog::activeDataChanged(int index)
{

	if (index == -1 || index >= m_settings.count()) {
		m_activeSetting = nullptr;
		return;
	}
	m_activeSetting = &(m_settings[index]);
	applySettings();
}

void Post2dWindowParticleStructuredSettingDialog::iMinChanged(int min)
{
	if (m_applying) {return;}
	if (ui->imaxSlider->value() < min) {
		ui->imaxSlider->setValue(min);
	}
	if (m_activeSetting != nullptr) {
		m_activeSetting->range.iMin = min - 1;
	}
}

void Post2dWindowParticleStructuredSettingDialog::iMaxChanged(int max)
{
	if (m_applying) {return;}
	if (ui->iminSlider->value() > max) {
		ui->iminSlider->setValue(max);
	}
	if (m_activeSetting != nullptr) {
		m_activeSetting->range.iMax = max - 1;
	}
}

void Post2dWindowParticleStructuredSettingDialog::jMinChanged(int min)
{
	if (m_applying) {return;}
	if (ui->jmaxSlider->value() < min) {
		ui->jmaxSlider->setValue(min);
	}
	if (m_activeSetting != nullptr) {
		m_activeSetting->range.jMin = min - 1;
	}
}

void Post2dWindowParticleStructuredSettingDialog::jMaxChanged(int max)
{
	if (m_applying) {return;}
	if (ui->jminSlider->value() > max) {
		ui->jminSlider->setValue(max);
	}
	if (m_activeSetting != nullptr) {
		m_activeSetting->range.jMax = max - 1;
	}
}

void Post2dWindowParticleStructuredSettingDialog::handleTimeSliderChange(int val)
{
	if (val < m_skipNominations.count()) {
		ui->timeValueLabel->setText(QString("1/%1").arg(m_skipNominations.at(m_skipNominations.count() - val - 1)));
	} else if (val > m_skipNominations.count()) {
		ui->timeValueLabel->setText(QString("%1").arg(m_subDivNominations.at(val - m_skipNominations.count() - 1)));
	} else {
		ui->timeValueLabel->setText("1");
	}
}

void Post2dWindowParticleStructuredSettingDialog::handleSpaceSliderChange(int val)
{
	if (val < m_skipNominations.count()) {
		ui->spaceValueLabel->setText(QString("1/%1").arg(m_skipNominations.at(m_skipNominations.count() - val - 1)));
		if (m_activeSetting != nullptr) {
			m_activeSetting->spaceMode = Post2dWindowStructuredParticleSetSetting::smSkip;
			m_activeSetting->spaceSamplingRate = m_skipNominations.at(m_skipNominations.count() - ui->spaceSlider->value() - 1);
		}
	} else if (val > m_skipNominations.count()) {
		ui->spaceValueLabel->setText(QString("%1").arg(m_subDivNominations.at(val - m_skipNominations.count() - 1)));
		if (m_activeSetting != nullptr) {
			m_activeSetting->spaceMode = Post2dWindowStructuredParticleSetSetting::smSubdivide;
			m_activeSetting->spaceDivision = m_subDivNominations.at(ui->spaceSlider->value() - m_skipNominations.count() - 1);
		}
	} else {
		ui->spaceValueLabel->setText("1");
		if (m_activeSetting != nullptr) {
			m_activeSetting->spaceMode = Post2dWindowStructuredParticleSetSetting::smNormal;
		}
	}
}

void Post2dWindowParticleStructuredSettingDialog::colorChanged(const QColor& color)
{
	if (m_activeSetting == nullptr) {return;}
	m_activeSetting->color = color;
}

void Post2dWindowParticleStructuredSettingDialog::sizeChanged(int size)
{
	if (m_activeSetting == nullptr) {return;}
	m_activeSetting->size = size;
}

void Post2dWindowParticleStructuredSettingDialog::setupNominations()
{
	int noms[] = {2, 3, 4, 5, 0};
	int idx = 0;
	while (noms[idx] != 0) {
		m_subDivNominations.append(noms[idx]);
		m_skipNominations.append(noms[idx]);
		++ idx;
	}
	ui->timeSlider->setMaximum(m_subDivNominations.count() + m_skipNominations.count());
	ui->spaceSlider->setMaximum(m_subDivNominations.count() + m_skipNominations.count());

	ui->spaceMinLabel->setText(QString("1/%1").arg(m_skipNominations.last()));
	ui->timeMinLabel->setText(QString("1/%1").arg(m_skipNominations.last()));

	ui->spaceMaxLabel->setText(QString("%1").arg(m_subDivNominations.last()));
	ui->timeMaxLabel->setText(QString("%1").arg(m_subDivNominations.last()));
}

void Post2dWindowParticleStructuredSettingDialog::setupSolutionComboBox(PostZoneDataContainer* zoneData)
{
	vtkPointData* pd = zoneData->data()->GetPointData();
	SolverDefinitionGridType* gt = zoneData->gridType();
	int num = pd->GetNumberOfArrays();
	ui->solutionComboBox->blockSignals(true);
	for (int i = 0; i < num; ++i) {
		vtkAbstractArray* tmparray = pd->GetArray(i);
		if (tmparray == nullptr) {continue;}
		QString name = tmparray->GetName();
		if (tmparray->GetNumberOfComponents() <= 1) {
			// scalar attributes.
			continue;
		}
		ui->solutionComboBox->addItem(gt->solutionCaption(name));
		m_solutions.append(name);
	}
	ui->solutionComboBox->blockSignals(false);
	if (m_solutions.count() < 2) {
		// solution selection is needless.
		ui->physValLabel->hide();
		ui->solutionComboBox->hide();
	}
}

void Post2dWindowParticleStructuredSettingDialog::setupSettingList()
{
	for (int i = 0; i < m_settings.count(); ++i) {
		ui->startPositionListWidget->addItem(QString("%1").arg(i + 1));
	}
	// select the first one.
	ui->startPositionListWidget->setCurrentRow(0);
	updateRemoveButtonStatus();
}

void Post2dWindowParticleStructuredSettingDialog::applySettings()
{
	if (m_activeSetting == nullptr) {return;}
	m_applying = true;
	ui->iminSlider->setValue(m_activeSetting->range.iMin + 1);
	ui->imaxSlider->setValue(m_activeSetting->range.iMax + 1);
	ui->jminSlider->setValue(m_activeSetting->range.jMin + 1);
	ui->jmaxSlider->setValue(m_activeSetting->range.jMax + 1);
	if (m_activeSetting->spaceMode == Post2dWindowStructuredParticleSetSetting::smNormal) {
		ui->spaceSlider->setValue(m_skipNominations.count());
		handleSpaceSliderChange(m_skipNominations.count());
	} else if (m_activeSetting->spaceMode == Post2dWindowStructuredParticleSetSetting::smSkip) {
		for (int i = 0; i < m_skipNominations.count(); ++i) {
			if (m_skipNominations.at(i) == m_activeSetting->spaceSamplingRate) {
				ui->spaceSlider->setValue(m_skipNominations.count() - i - 1);
				handleSpaceSliderChange(m_skipNominations.count() - i - 1);
			}
		}
	} else if (m_activeSetting->spaceMode == Post2dWindowStructuredParticleSetSetting::smSubdivide) {
		for (int i = 0; i < m_subDivNominations.count(); ++i) {
			if (m_subDivNominations.at(i) == m_activeSetting->spaceDivision) {
				ui->spaceSlider->setValue(m_skipNominations.count() + i + 1);
				handleSpaceSliderChange(m_skipNominations.count() + i + 1);
			}
		}
	}
	ui->colorWidget->setColor(m_activeSetting->color);
	ui->sizeSpinBox->setValue(m_activeSetting->size);
	m_applying = false;
}

void Post2dWindowParticleStructuredSettingDialog::addData()
{
	if (m_activeSetting == nullptr) {return;}
	Post2dWindowStructuredParticleSetSetting setting = *m_activeSetting;
	m_settings.append(setting);
	QListWidgetItem* tmpitem = ui->startPositionListWidget->item(ui->startPositionListWidget->count() - 1);
	int tmpint = tmpitem->text().toInt();
	++ tmpint;
	ui->startPositionListWidget->addItem(QString("%1").arg(tmpint));
	ui->startPositionListWidget->setCurrentRow(ui->startPositionListWidget->count() - 1);
	updateRemoveButtonStatus();
}

void Post2dWindowParticleStructuredSettingDialog::removeData()
{
	int current = ui->startPositionListWidget->currentRow();
	ui->startPositionListWidget->blockSignals(true);
	QListWidgetItem* item = ui->startPositionListWidget->takeItem(current);
	if (item != nullptr) {delete item;}
	ui->startPositionListWidget->blockSignals(false);
	m_settings.removeAt(current);
	if (current >= m_settings.count()) {current = m_settings.count() - 1;}
	ui->startPositionListWidget->setCurrentRow(current);
	m_activeSetting = &(m_settings[current]);
	applySettings();
	updateRemoveButtonStatus();
}

void Post2dWindowParticleStructuredSettingDialog::showRegionDialog()
{
	Post2dGridRegionSelectDialog dialog(this);
	if (! m_activeAvailable) {
		dialog.disableActive();
	}
	dialog.hideCustom();
	dialog.setRegionMode(m_regionMode);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}
	m_regionMode = dialog.regionMode();
}

void Post2dWindowParticleStructuredSettingDialog::updateRemoveButtonStatus()
{
	ui->removePushButton->setEnabled(m_settings.count() > 1);
}
