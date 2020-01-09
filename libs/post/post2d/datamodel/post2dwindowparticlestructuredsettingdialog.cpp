#include "ui_post2dwindowparticlestructuredsettingdialog.h"

#include "../post2dgridregionselectdialog.h"
#include "post2dwindowparticlestructuredsettingdialog.h"

#include <guibase/comboboxtool.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>
#include <postbase/particle/particlearbitrarytimeeditdialog.h>

#include <vtkPointData.h>
#include <vtkStructuredGrid.h>

Post2dWindowParticleStructuredSettingDialog::Post2dWindowParticleStructuredSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui {new Ui::Post2dWindowParticleStructuredSettingDialog},
	m_applying {false}
{
	ui->setupUi(this);

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
	connect(ui->arbitraryEditButton, SIGNAL(clicked()), this, SLOT(editArbitraryTimes()));
}

Post2dWindowParticleStructuredSettingDialog::~Post2dWindowParticleStructuredSettingDialog()
{
	delete ui;
}

void Post2dWindowParticleStructuredSettingDialog::setProjectMainFile(ProjectMainFile* file)
{
	m_mainFile = file;
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

void Post2dWindowParticleStructuredSettingDialog::setSettings(const Post2dWindowNodeVectorParticleGroupDataItem::Setting& s, const QList<Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting>& sts)
{
	m_setting = s;
	m_stSettings = sts;

	// solution
	auto it = std::find(m_solutions.begin(), m_solutions.end(), iRIC::toStr(s.target));
	if (it == m_solutions.end()) {it = m_solutions.begin();}
	ui->solutionComboBox->setCurrentIndex(it - m_solutions.begin());

	if (s.generateMode == Post2dWindowNodeVectorParticleGroupDataItem::gmPeriodical) {
		ui->periodicalRadioButton->setChecked(true);
	} else {
		ui->arbitraryRadioButton->setChecked(true);
	}

	// timemode
	if (s.timeMode == Post2dWindowNodeVectorParticleGroupDataItem::tmNormal) {
		ui->timeSlider->setValue(m_skipNominations.count());
	} else if (s.timeMode == Post2dWindowNodeVectorParticleGroupDataItem::tmSkip) {
		for (int i = 0; i < m_skipNominations.count(); ++i) {
			if (m_skipNominations.at(i) == s.timeSamplingRate) {
				ui->timeSlider->setValue(m_skipNominations.count() - i - 1);
			}
		}
	} else if (s.timeMode == Post2dWindowNodeVectorParticleGroupDataItem::tmSubdivide) {
		for (int i = 0; i < m_subDivNominations.count(); ++i) {
			if (m_subDivNominations.at(i) == s.timeDivision) {
				ui->timeSlider->setValue(m_skipNominations.count() + i + 1);
			}
		}
	}

	setupSettingList();
}

Post2dWindowNodeVectorParticleGroupDataItem::Setting Post2dWindowParticleStructuredSettingDialog::setting() const
{
	Post2dWindowNodeVectorParticleGroupDataItem::Setting ret = m_setting;

	// solution
	int index = ui->solutionComboBox->currentIndex();
	ret.target = m_solutions.at(index).c_str();

	if (ui->periodicalRadioButton->isChecked()) {
		ret.generateMode = Post2dWindowNodeVectorParticleGroupDataItem::gmPeriodical;
	} else {
		ret.generateMode = Post2dWindowNodeVectorParticleGroupDataItem::gmArbitrary;
	}

	// timemode
	if (ui->timeSlider->value() == m_skipNominations.count()) {
		ret.timeMode = Post2dWindowNodeVectorParticleGroupDataItem::tmNormal;
	} else if (ui->timeSlider->value() < m_skipNominations.count()) {
		ret.timeMode = Post2dWindowNodeVectorParticleGroupDataItem::tmSkip;
		ret.timeSamplingRate = m_skipNominations.at(m_skipNominations.count() - ui->timeSlider->value() - 1);
	} else {
		ret.timeMode = Post2dWindowNodeVectorParticleGroupDataItem::tmSubdivide;
		ret.timeDivision = m_subDivNominations.at(ui->timeSlider->value() - m_skipNominations.count() - 1);
	}
	return ret;
}

void Post2dWindowParticleStructuredSettingDialog::activeDataChanged(int index)
{

	if (index == -1 || index >= m_stSettings.count()) {
		m_activeSetting = nullptr;
		return;
	}
	m_activeSetting = &(m_stSettings[index]);
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
			m_activeSetting->spaceMode = Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting::smSkip;
			m_activeSetting->spaceSamplingRate = m_skipNominations.at(m_skipNominations.count() - ui->spaceSlider->value() - 1);
		}
	} else if (val > m_skipNominations.count()) {
		ui->spaceValueLabel->setText(QString("%1").arg(m_subDivNominations.at(val - m_skipNominations.count() - 1)));
		if (m_activeSetting != nullptr) {
			m_activeSetting->spaceMode = Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting::smSubdivide;
			m_activeSetting->spaceDivision = m_subDivNominations.at(ui->spaceSlider->value() - m_skipNominations.count() - 1);
		}
	} else {
		ui->spaceValueLabel->setText("1");
		if (m_activeSetting != nullptr) {
			m_activeSetting->spaceMode = Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting::smNormal;
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

	m_solutions = vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(pd);
	ComboBoxTool::setupItems(gt->solutionCaptions(m_solutions), ui->solutionComboBox);

	if (m_solutions.size() < 2) {
		// solution selection is needless.
		ui->physValLabel->hide();
		ui->solutionComboBox->hide();
	}
}

void Post2dWindowParticleStructuredSettingDialog::setupSettingList()
{
	for (int i = 0; i < m_stSettings.count(); ++i) {
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
	if (m_activeSetting->spaceMode == Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting::smNormal) {
		ui->spaceSlider->setValue(m_skipNominations.count());
		handleSpaceSliderChange(m_skipNominations.count());
	} else if (m_activeSetting->spaceMode == Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting::smSkip) {
		for (int i = 0; i < m_skipNominations.count(); ++i) {
			if (m_skipNominations.at(i) == m_activeSetting->spaceSamplingRate) {
				ui->spaceSlider->setValue(m_skipNominations.count() - i - 1);
				handleSpaceSliderChange(m_skipNominations.count() - i - 1);
			}
		}
	} else if (m_activeSetting->spaceMode == Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting::smSubdivide) {
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
	Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting setting = *m_activeSetting;
	m_stSettings.append(setting);
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
	m_stSettings.removeAt(current);
	if (current >= m_stSettings.count()) {current = m_stSettings.count() - 1;}
	ui->startPositionListWidget->setCurrentRow(current);
	m_activeSetting = &(m_stSettings[current]);
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
	dialog.setRegionMode(m_setting.regionMode);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}
	m_setting.regionMode = dialog.regionMode();
}

void Post2dWindowParticleStructuredSettingDialog::editArbitraryTimes()
{
	ParticleArbitraryTimeEditDialog dialog(this);
	dialog.setMainFile(m_mainFile);
	dialog.setTimeSteps(m_setting.arbitraryTimes.value());
	int ret = dialog.exec();

	if (ret == QDialog::Rejected) {return;}

	m_setting.arbitraryTimes.setValue(dialog.timeSteps());
}

void Post2dWindowParticleStructuredSettingDialog::updateRemoveButtonStatus()
{
	ui->removePushButton->setEnabled(m_stSettings.count() > 1);
}
