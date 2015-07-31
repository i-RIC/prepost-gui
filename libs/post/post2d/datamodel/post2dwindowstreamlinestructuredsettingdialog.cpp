#include "ui_post2dwindowstreamlinestructuredsettingdialog.h"

#include "../post2dgridregionselectdialog.h"
#include "post2dwindowstreamlinestructuredsettingdialog.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>

#include <vtkPointData.h>
#include <vtkStructuredGrid.h>

Post2dWindowStreamlineStructuredSettingDialog::Post2dWindowStreamlineStructuredSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Post2dWindowStreamlineStructuredSettingDialog)
{
	ui->setupUi(this);

	setupNominations();
	ui->spaceSlider->setTracking(true);
	m_applying = false;

	connect(ui->startPositionListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(activeDataChanged(int)));
	connect(ui->iminSlider, SIGNAL(valueChanged(int)), this, SLOT(iMinChanged(int)));
	connect(ui->imaxSlider, SIGNAL(valueChanged(int)), this, SLOT(iMaxChanged(int)));
	connect(ui->jminSlider, SIGNAL(valueChanged(int)), this, SLOT(jMinChanged(int)));
	connect(ui->jmaxSlider, SIGNAL(valueChanged(int)), this, SLOT(jMaxChanged(int)));
	connect(ui->spaceSlider, SIGNAL(valueChanged(int)), this, SLOT(handleSpaceSliderChange(int)));
	connect(ui->colorWidget, SIGNAL(colorChanged(QColor)), this, SLOT(colorChanged(QColor)));
	connect(ui->widthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(widthChanged(int)));

	connect(ui->addPushButton, SIGNAL(clicked()), this, SLOT(addData()));
	connect(ui->removePushButton, SIGNAL(clicked()), this, SLOT(removeData()));
	connect(ui->regionSettingButton, SIGNAL(clicked()), this, SLOT(showRegionDialog()));
}

Post2dWindowStreamlineStructuredSettingDialog::~Post2dWindowStreamlineStructuredSettingDialog()
{
	delete ui;
}

void Post2dWindowStreamlineStructuredSettingDialog::setZoneData(PostZoneDataContainer* zoneData)
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

void Post2dWindowStreamlineStructuredSettingDialog::setSolution(const QString& sol)
{
	int index = m_solutions.indexOf(sol);
	if (index == -1) {index = 0;}
	ui->solutionComboBox->setCurrentIndex(index);
}

QString Post2dWindowStreamlineStructuredSettingDialog::solution() const
{
	int index = ui->solutionComboBox->currentIndex();
	return m_solutions.at(index);
}

void Post2dWindowStreamlineStructuredSettingDialog::activeDataChanged(int index)
{
	if (index == -1 || index >= m_settings.count()) {
		m_activeSetting = nullptr;
		return;
	}
	m_activeSetting = &(m_settings[index]);
	applySettings();
}

void Post2dWindowStreamlineStructuredSettingDialog::iMinChanged(int min)
{
	if (m_applying) {return;}
	if (ui->imaxSlider->value() < min) {
		ui->imaxSlider->setValue(min);
	}
	if (m_activeSetting != nullptr) {
		m_activeSetting->range.iMin = min - 1;
	}
}

void Post2dWindowStreamlineStructuredSettingDialog::iMaxChanged(int max)
{
	if (m_applying) {return;}
	if (ui->iminSlider->value() > max) {
		ui->iminSlider->setValue(max);
	}
	if (m_activeSetting != nullptr) {
		m_activeSetting->range.iMax = max - 1;
	}
}

void Post2dWindowStreamlineStructuredSettingDialog::jMinChanged(int min)
{
	if (m_applying) {return;}
	if (ui->jmaxSlider->value() < min) {
		ui->jmaxSlider->setValue(min);
	}
	if (m_activeSetting != nullptr) {
		m_activeSetting->range.jMin = min - 1;
	}
}

void Post2dWindowStreamlineStructuredSettingDialog::jMaxChanged(int max)
{
	if (m_applying) {return;}
	if (ui->jminSlider->value() > max) {
		ui->jminSlider->setValue(max);
	}
	if (m_activeSetting != nullptr) {
		m_activeSetting->range.jMax = max - 1;
	}
}

void Post2dWindowStreamlineStructuredSettingDialog::handleSpaceSliderChange(int val)
{
	if (val < m_skipNominations.count()) {
		ui->spaceValueLabel->setText(QString("1/%1").arg(m_skipNominations.at(m_skipNominations.count() - val - 1)));
		if (m_activeSetting != nullptr) {
			m_activeSetting->spaceMode = Post2dWindowStructuredStreamlineSetSetting::smSkip;
			m_activeSetting->spaceSamplingRate = m_skipNominations.at(m_skipNominations.count() - ui->spaceSlider->value() - 1);
		}
	} else if (val > m_skipNominations.count()) {
		ui->spaceValueLabel->setText(QString("%1").arg(m_subDivNominations.at(val - m_skipNominations.count() - 1)));
		if (m_activeSetting != nullptr) {
			m_activeSetting->spaceMode = Post2dWindowStructuredStreamlineSetSetting::smSubdivide;
			m_activeSetting->spaceDivision = m_subDivNominations.at(ui->spaceSlider->value() - m_skipNominations.count() - 1);
		}
	} else {
		ui->spaceValueLabel->setText("1");
		if (m_activeSetting != nullptr) {
			m_activeSetting->spaceMode = Post2dWindowStructuredStreamlineSetSetting::smNormal;
		}
	}
}

void Post2dWindowStreamlineStructuredSettingDialog::colorChanged(const QColor& color)
{
	if (m_activeSetting == nullptr) {return;}
	m_activeSetting->color = color;
}

void Post2dWindowStreamlineStructuredSettingDialog::widthChanged(int width)
{
	if (m_activeSetting == nullptr) {return;}
	m_activeSetting->width = width;
}

void Post2dWindowStreamlineStructuredSettingDialog::setupNominations()
{
	int noms[] = {2, 3, 4, 5, 0};
	int idx = 0;
	while (noms[idx] != 0) {
		m_subDivNominations.append(noms[idx]);
		m_skipNominations.append(noms[idx]);
		++ idx;
	}
	ui->spaceSlider->setMaximum(m_subDivNominations.count() + m_skipNominations.count());
	ui->spaceMinLabel->setText(QString("1/%1").arg(m_skipNominations.last()));
	ui->spaceMaxLabel->setText(QString("%1").arg(m_subDivNominations.last()));
}



void Post2dWindowStreamlineStructuredSettingDialog::setupSolutionComboBox(PostZoneDataContainer* zoneData)
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
	if (m_solutions.count() <= 1) {
		ui->physValLabel->hide();
		ui->solutionComboBox->hide();
	}
}

void Post2dWindowStreamlineStructuredSettingDialog::setupSettingList()
{
	for (int i = 0; i < m_settings.count(); ++i) {
		ui->startPositionListWidget->addItem(QString("%1").arg(i + 1));
	}
	// select the first one.
	ui->startPositionListWidget->setCurrentRow(0);
	updateRemoveButtonStatus();
}

void Post2dWindowStreamlineStructuredSettingDialog::applySettings()
{
	if (m_activeSetting == nullptr) {return;}
	m_applying = true;
	ui->iminSlider->setValue(m_activeSetting->range.iMin + 1);
	ui->imaxSlider->setValue(m_activeSetting->range.iMax + 1);
	ui->jminSlider->setValue(m_activeSetting->range.jMin + 1);
	ui->jmaxSlider->setValue(m_activeSetting->range.jMax + 1);
	if (m_activeSetting->spaceMode == Post2dWindowStructuredStreamlineSetSetting::smNormal) {
		ui->spaceSlider->setValue(m_skipNominations.count());
		handleSpaceSliderChange(m_skipNominations.count());
	} else if (m_activeSetting->spaceMode == Post2dWindowStructuredStreamlineSetSetting::smSkip) {
		for (int i = 0; i < m_skipNominations.count(); ++i) {
			if (m_skipNominations.at(i) == m_activeSetting->spaceSamplingRate) {
				ui->spaceSlider->setValue(m_skipNominations.count() - i - 1);
				handleSpaceSliderChange(m_skipNominations.count() - i - 1);
			}
		}
	} else if (m_activeSetting->spaceMode == Post2dWindowStructuredStreamlineSetSetting::smSubdivide) {
		for (int i = 0; i < m_subDivNominations.count(); ++i) {
			if (m_subDivNominations.at(i) == m_activeSetting->spaceDivision) {
				ui->spaceSlider->setValue(m_skipNominations.count() + i + 1);
				handleSpaceSliderChange(m_skipNominations.count() + i + 1);
			}
		}
	}
	ui->colorWidget->setColor(m_activeSetting->color);
	ui->widthSpinBox->setValue(m_activeSetting->width);
	m_applying = false;
}

void Post2dWindowStreamlineStructuredSettingDialog::addData()
{
	if (m_activeSetting == nullptr) {return;}
	Post2dWindowStructuredStreamlineSetSetting setting = *m_activeSetting;
	m_settings.append(setting);
	QListWidgetItem* tmpitem = ui->startPositionListWidget->item(ui->startPositionListWidget->count() - 1);
	int tmpint = tmpitem->text().toInt();
	++ tmpint;
	ui->startPositionListWidget->addItem(QString("%1").arg(tmpint));
	ui->startPositionListWidget->setCurrentRow(ui->startPositionListWidget->count() - 1);
	updateRemoveButtonStatus();
}

void Post2dWindowStreamlineStructuredSettingDialog::removeData()
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

void Post2dWindowStreamlineStructuredSettingDialog::showRegionDialog()
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

void Post2dWindowStreamlineStructuredSettingDialog::updateRemoveButtonStatus()
{
	ui->removePushButton->setEnabled(m_settings.count() > 1);
}
