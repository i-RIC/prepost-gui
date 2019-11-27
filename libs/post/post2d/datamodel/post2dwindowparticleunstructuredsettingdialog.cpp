#include "ui_post2dwindowparticleunstructuredsettingdialog.h"

#include "../post2dgridregionselectdialog.h"
#include "post2dwindowparticleunstructuredsettingdialog.h"
#include "private/post2dwindownodevectorparticlegroupunstructureddataitem_setsettingcommand.h"

#include <guibase/comboboxtool.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <postbase/particle/particlearbitrarytimeeditdialog.h>

#include <QPushButton>
#include <QVector2D>

#include <vtkPointData.h>
#include <vtkStructuredGrid.h>

Post2dWindowParticleUnstructuredSettingDialog::Post2dWindowParticleUnstructuredSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Post2dWindowParticleUnstructuredSettingDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);

	setupNominations();

	ui->timeSlider->setTracking(true);

	connect(ui->startPositionListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(activeDataChanged(int)));
	connect(ui->timeSlider, SIGNAL(valueChanged(int)), this, SLOT(handleTimeSliderChange(int)));
	connect(ui->point1XEdit, SIGNAL(textChanged(QString)), this, SLOT(pointsEdited()));
	connect(ui->point1YEdit, SIGNAL(textChanged(QString)), this, SLOT(pointsEdited()));
	connect(ui->point2XEdit, SIGNAL(textChanged(QString)), this, SLOT(pointsEdited()));
	connect(ui->point2YEdit, SIGNAL(textChanged(QString)), this, SLOT(pointsEdited()));
	connect(ui->numPointsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(numPointsChanged(int)));
	connect(ui->colorWidget, SIGNAL(colorChanged(QColor)), this, SLOT(colorChanged(QColor)));
	connect(ui->sizeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(sizeChanged(int)));
	connect(ui->addPushButton, SIGNAL(clicked()), this, SLOT(addData()));
	connect(ui->removePushButton, SIGNAL(clicked()), this, SLOT(removeData()));
	connect(ui->regionSettingButton, SIGNAL(clicked()), this, SLOT(showRegionDialog()));
	connect(ui->arbitraryEditButton, SIGNAL(clicked()), this, SLOT(editArbitraryTimes()));

	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonPress(QAbstractButton*)));

	m_pressed = false;
	m_applying = false;
}

Post2dWindowParticleUnstructuredSettingDialog::~Post2dWindowParticleUnstructuredSettingDialog()
{
	delete ui;
}

void Post2dWindowParticleUnstructuredSettingDialog::setProjectMainFile(ProjectMainFile* file)
{
	m_mainFile = file;
}

void Post2dWindowParticleUnstructuredSettingDialog::setZoneData(PostZoneDataContainer* zoneData)
{
	setupSolutionComboBox(zoneData);
}

void Post2dWindowParticleUnstructuredSettingDialog::setSettings(const Post2dWindowNodeVectorParticleGroupDataItem::Setting& s, const QList<Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting> & unsts)
{
	m_setting = s;
	m_unstSettings = unsts;

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
			if (m_subDivNominations.at(i) == s.timeDivision ) {
				ui->timeSlider->setValue(m_skipNominations.count() + i + 1);
			}
		}
	}

	setupSettingList();
}

Post2dWindowNodeVectorParticleGroupDataItem::Setting Post2dWindowParticleUnstructuredSettingDialog::setting() const
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

	// timeMode
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

void Post2dWindowParticleUnstructuredSettingDialog::informButtonDown(const QPointF &p)
{
	if (ui->pointsMouseRadioButton->isChecked()) {
		ui->point1XEdit->setValue(p.x());
		ui->point1YEdit->setValue(p.y());
		ui->point2XEdit->setValue(p.x());
		ui->point2YEdit->setValue(p.y());
	}
	m_pressed = true;
	apply();
}

void Post2dWindowParticleUnstructuredSettingDialog::informButtonUp(const QPointF &p)
{
	if (ui->pointsMouseRadioButton->isChecked()) {
		ui->point2XEdit->setValue(p.x());
		ui->point2YEdit->setValue(p.y());
	}
	m_pressed = false;
	apply();
}

void Post2dWindowParticleUnstructuredSettingDialog::updateMousePosition(const QVector2D& p)
{
	if (ui->pointsMouseRadioButton->isChecked() && m_pressed) {
		ui->point2XEdit->setValue(p.x());
		ui->point2YEdit->setValue(p.y());
		apply();
	}
}

void Post2dWindowParticleUnstructuredSettingDialog::accept()
{
	m_dataItem->hidePreviewSetting();
	m_dataItem->pushRenderCommand(new Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::SetSettingCommand(setting(), unstSettings(), m_dataItem), m_dataItem, true);
	QDialog::accept();
}

void Post2dWindowParticleUnstructuredSettingDialog::reject()
{
	m_dataItem->hidePreviewSetting();
	m_dataItem->renderGraphicsView();
	QDialog::reject();
}

void Post2dWindowParticleUnstructuredSettingDialog::activeDataChanged(int index)
{
	if (index == -1 || index >= m_unstSettings.count()) {
		m_activeSetting = nullptr;
		return;
	}
	m_activeSetting = &(m_unstSettings[index]);
	applySettings();
}

void Post2dWindowParticleUnstructuredSettingDialog::handleTimeSliderChange(int val)
{
	if (val < m_skipNominations.count()) {
		ui->timeValueLabel->setText(QString("1/%1").arg(m_skipNominations.at(m_skipNominations.count() - val - 1)));
	} else if (val > m_skipNominations.count()) {
		ui->timeValueLabel->setText(QString("%1").arg(m_subDivNominations.at(val - m_skipNominations.count() - 1)));
	} else {
		ui->timeValueLabel->setText("1");
	}
}

void Post2dWindowParticleUnstructuredSettingDialog::pointsEdited()
{
	if (m_applying) {return;}
	if (ui->point1XEdit->text() == "") {return;}
	if (ui->point1YEdit->text() == "") {return;}
	if (ui->point2XEdit->text() == "") {return;}
	if (ui->point2YEdit->text() == "") {return;}
	if (m_activeSetting == nullptr) {return;}
	m_activeSetting->point1 = QPointF(ui->point1XEdit->value(), ui->point1YEdit->value());
	m_activeSetting->point2 = QPointF(ui->point2XEdit->value(), ui->point2YEdit->value());

	m_activeSetting->pointsSet = true;
}

void Post2dWindowParticleUnstructuredSettingDialog::handleButtonPress(QAbstractButton* button)
{
	if (button == ui->buttonBox->button(QDialogButtonBox::Apply)) {
		apply();
	}
}

void Post2dWindowParticleUnstructuredSettingDialog::numPointsChanged(int num)
{
	if (m_activeSetting == nullptr) {return;}
	m_activeSetting->numberOfPoints = num;
}

void Post2dWindowParticleUnstructuredSettingDialog::colorChanged(const QColor& color)
{
	if (m_activeSetting == nullptr) {return;}
	m_activeSetting->color = color;
}

void Post2dWindowParticleUnstructuredSettingDialog::sizeChanged(int size)
{
	if (m_activeSetting == nullptr) {return;}
	m_activeSetting->size = size;
}

void Post2dWindowParticleUnstructuredSettingDialog::addData()
{
	if (m_activeSetting == nullptr) {return;}
	Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting setting = *m_activeSetting;
	m_unstSettings.append(setting);
	QListWidgetItem* tmpitem = ui->startPositionListWidget->item(ui->startPositionListWidget->count() - 1);
	int tmpint = tmpitem->text().toInt();
	++ tmpint;
	ui->startPositionListWidget->addItem(QString("%1").arg(tmpint));
	ui->startPositionListWidget->setCurrentRow(ui->startPositionListWidget->count() - 1);
	updateRemoveButtonStatus();
}

void Post2dWindowParticleUnstructuredSettingDialog::removeData()
{
	int current = ui->startPositionListWidget->currentRow();
	ui->startPositionListWidget->blockSignals(true);
	QListWidgetItem* item = ui->startPositionListWidget->takeItem(current);
	if (item != nullptr) {delete item;}
	ui->startPositionListWidget->blockSignals(false);
	m_unstSettings.removeAt(current);
	if (current >= m_unstSettings.count()) {current = m_unstSettings.count() - 1;}
	ui->startPositionListWidget->setCurrentRow(current);
	m_activeSetting = &(m_unstSettings[current]);
	applySettings();
	updateRemoveButtonStatus();
}

void Post2dWindowParticleUnstructuredSettingDialog::showRegionDialog()
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

void Post2dWindowParticleUnstructuredSettingDialog::editArbitraryTimes()
{
	ParticleArbitraryTimeEditDialog dialog(this);
	dialog.setMainFile(m_mainFile);
	dialog.setTimeSteps(m_setting.arbitraryTimes.value());
	int ret = dialog.exec();

	if (ret == QDialog::Rejected) {return;}

	m_setting.arbitraryTimes.setValue(dialog.timeSteps());
}

void Post2dWindowParticleUnstructuredSettingDialog::apply()
{
	QPointF v1(ui->point1XEdit->value(), ui->point1YEdit->value());
	QPointF v2(ui->point2XEdit->value(), ui->point2YEdit->value());
	m_dataItem->setSetting(v1, v2, ui->numPointsSpinBox->value(), ui->sizeSpinBox->value());
}

void Post2dWindowParticleUnstructuredSettingDialog::setupNominations()
{
	int noms[] = {2, 3, 4, 5, 0};
	int idx = 0;
	while (noms[idx] != 0) {
		m_subDivNominations.append(noms[idx]);
		m_skipNominations.append(noms[idx]);
		++ idx;
	}
	ui->timeSlider->setMaximum(m_subDivNominations.count() + m_skipNominations.count());
	ui->timeMinLabel->setText(QString("1/%1").arg(m_skipNominations.last()));
	ui->timeMaxLabel->setText(QString("%1").arg(m_subDivNominations.last()));
}

void Post2dWindowParticleUnstructuredSettingDialog::setupSolutionComboBox(PostZoneDataContainer* zoneData)
{
	vtkPointData* pd = zoneData->data()->GetPointData();
	SolverDefinitionGridType* gt = zoneData->gridType();

	m_solutions = vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(pd);
	ComboBoxTool::setupItems(m_solutions, ui->solutionComboBox);

	if (m_solutions.size() < 2) {
		ui->physValLabel->hide();
		ui->solutionComboBox->hide();
	}
}

void Post2dWindowParticleUnstructuredSettingDialog::setupSettingList()
{
	for (int i = 0; i < m_unstSettings.count(); ++i) {
		ui->startPositionListWidget->addItem(QString("%1").arg(i + 1));
	}
	// select the first one.
	ui->startPositionListWidget->setCurrentRow(0);
	updateRemoveButtonStatus();
}

void Post2dWindowParticleUnstructuredSettingDialog::applySettings()
{
	if (m_activeSetting == nullptr) {return;}
	m_applying = true;
	ui->pointsMouseRadioButton->setChecked(true);
	if (m_activeSetting->pointsSet) {
		ui->point1XEdit->setValue(m_activeSetting->point1.value().x());
		ui->point1YEdit->setValue(m_activeSetting->point1.value().y());
		ui->point2XEdit->setValue(m_activeSetting->point2.value().x());
		ui->point2YEdit->setValue(m_activeSetting->point2.value().y());
	} else {
		ui->point1XEdit->clear();
		ui->point1YEdit->clear();
		ui->point2XEdit->clear();
		ui->point2YEdit->clear();
	}
	ui->numPointsSpinBox->setValue(m_activeSetting->numberOfPoints);
	ui->colorWidget->setColor(m_activeSetting->color);
	ui->sizeSpinBox->setValue(m_activeSetting->size);
	m_applying = false;
	apply();
}

void Post2dWindowParticleUnstructuredSettingDialog::updateRemoveButtonStatus()
{
	ui->removePushButton->setEnabled(m_unstSettings.count() > 1);
}
