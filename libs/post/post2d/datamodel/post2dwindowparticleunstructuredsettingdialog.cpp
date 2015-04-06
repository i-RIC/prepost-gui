#include "../post2dgridregionselectdialog.h"
#include "post2dwindowparticleunstructuredsettingdialog.h"
#include "ui_post2dwindowparticleunstructuredsettingdialog.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>

#include <QPushButton>

#include <vtkPointData.h>
#include <vtkStructuredGrid.h>

Post2dWindowParticleUnstructuredSettingDialog::Post2dWindowParticleUnstructuredSettingDialog(QWidget *parent) :
    QDialog(parent),
	ui(new Ui::Post2dWindowParticleUnstructuredSettingDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
    ui->setupUi(this);

	m_timeMode = Post2dWindowNodeVectorParticleGroupDataItem::tmNormal;
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

	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonPress(QAbstractButton*)));

	m_pressed = false;
	m_applying = false;
}

Post2dWindowParticleUnstructuredSettingDialog::~Post2dWindowParticleUnstructuredSettingDialog()
{
    delete ui;
}

void Post2dWindowParticleUnstructuredSettingDialog::setZoneData(PostZoneDataContainer* zoneData)
{
	setupSolutionComboBox(zoneData);
}

void Post2dWindowParticleUnstructuredSettingDialog::setSolution(const QString& sol)
{
	int index = m_solutions.indexOf(sol);
	if (index == -1){index = 0;}
	ui->solutionComboBox->setCurrentIndex(index);
}

const QString Post2dWindowParticleUnstructuredSettingDialog::solution()
{
	int index = ui->solutionComboBox->currentIndex();
	return m_solutions.at(index);
}

void Post2dWindowParticleUnstructuredSettingDialog::setTimeMode(Post2dWindowNodeVectorParticleGroupDataItem::TimeMode tm)
{
	m_timeMode = tm;
	if (m_timeMode == Post2dWindowNodeVectorParticleGroupDataItem::tmNormal){
		ui->timeSlider->setValue(m_skipNominations.count());
	}
}

void Post2dWindowParticleUnstructuredSettingDialog::setTimeSamplingRate(int sr)
{
	if (m_timeMode != Post2dWindowNodeVectorParticleGroupDataItem::tmSkip){return;}
	for (int i = 0; i < m_skipNominations.count(); ++i){
		if (m_skipNominations.at(i) == sr){
			ui->timeSlider->setValue(m_skipNominations.count() - i - 1);
			return;
		}
	}
}

void Post2dWindowParticleUnstructuredSettingDialog::setTimeDivision(int sd)
{
	if (m_timeMode != Post2dWindowNodeVectorParticleGroupDataItem::tmSubdivide){return;}
	for (int i = 0; i < m_subDivNominations.count(); ++i){
		if (m_subDivNominations.at(i) == sd){
			ui->timeSlider->setValue(m_skipNominations.count() + i + 1);
			return;
		}
	}
}

Post2dWindowNodeVectorParticleGroupDataItem::TimeMode Post2dWindowParticleUnstructuredSettingDialog::timeMode()
{
	if (ui->timeSlider->value() == m_skipNominations.count()){
		return Post2dWindowNodeVectorParticleGroupDataItem::tmNormal;
	} else if (ui->timeSlider->value() < m_skipNominations.count()){
		return Post2dWindowNodeVectorParticleGroupDataItem::tmSkip;
	} else{
		return Post2dWindowNodeVectorParticleGroupDataItem::tmSubdivide;
	}
}

int Post2dWindowParticleUnstructuredSettingDialog::timeSamplingRate()
{
	if (ui->timeSlider->value() >= m_skipNominations.count()){return 1;}
	return m_skipNominations.at(m_skipNominations.count() - ui->timeSlider->value() - 1);
}

int Post2dWindowParticleUnstructuredSettingDialog::timeDivision()
{
	if (ui->timeSlider->value() <= m_skipNominations.count()){return 1;}
	return m_subDivNominations.at(ui->timeSlider->value() - m_skipNominations.count() - 1);
}

void Post2dWindowParticleUnstructuredSettingDialog::informButtonDown(const QVector2D& p)
{
	if (ui->pointsMouseRadioButton->isChecked()){
		ui->point1XEdit->setValue(p.x());
		ui->point1YEdit->setValue(p.y());
		ui->point2XEdit->setValue(p.x());
		ui->point2YEdit->setValue(p.y());
	}
	m_pressed = true;
	apply();
}

void Post2dWindowParticleUnstructuredSettingDialog::informButtonUp(const QVector2D& p)
{
	if (ui->pointsMouseRadioButton->isChecked()){
		ui->point2XEdit->setValue(p.x());
		ui->point2YEdit->setValue(p.y());
	}
	m_pressed = false;
	apply();
}

void Post2dWindowParticleUnstructuredSettingDialog::updateMousePosition(const QVector2D& p)
{
	if (ui->pointsMouseRadioButton->isChecked() && m_pressed){
		ui->point2XEdit->setValue(p.x());
		ui->point2YEdit->setValue(p.y());
		apply();
	}
}

class Post2dWindowParticleUnstructuredSetProperty : public QUndoCommand
{
public:
	Post2dWindowParticleUnstructuredSetProperty(const QString& sol, Post2dWindowNodeVectorParticleGroupDataItem::TimeMode tm, int tsr, int tdiv, const QList<Post2dWindowUnstructuredParticleSetSetting>& settings, StructuredGridRegion::RegionMode rm, Post2dWindowNodeVectorParticleGroupUnstructuredDataItem* item)
		: QUndoCommand(QObject::tr("Update Particle Setting"))
	{
		m_newEnabled = true;
		m_newSolution = sol;
		m_newTimeMode = tm;
		m_newTimeSamplingRate = tsr;
		m_newTimeDivision = tdiv;
		m_newSettings = settings;
		m_newRegionMode = rm;

		m_oldEnabled = item->isEnabled();
		m_oldSolution = item->m_currentSolution;
		m_oldTimeMode = item->m_timeMode;
		m_oldTimeSamplingRate = item->m_timeSamplingRate;
		m_oldTimeDivision = item->m_timeDivision;
		m_oldSettings = item->m_settings;
		m_oldRegionMode = item->m_regionMode;

		m_item = item;
	}
	void undo()
	{
		m_item->setIsCommandExecuting(true);
		m_item->setEnabled(m_oldEnabled);
		m_item->setCurrentSolution(m_oldSolution);
		m_item->m_timeMode = m_oldTimeMode;
		m_item->m_timeSamplingRate = m_oldTimeSamplingRate;
		m_item->m_timeDivision = m_oldTimeDivision;
		m_item->m_settings = m_oldSettings;
		m_item->m_regionMode = m_oldRegionMode;

		m_item->updateActorSettings();
		m_item->setIsCommandExecuting(false);
	}
	void redo()
	{
		m_item->setIsCommandExecuting(true);
		m_item->setEnabled(m_newEnabled);
		m_item->setCurrentSolution(m_newSolution);
		m_item->m_timeMode = m_newTimeMode;
		m_item->m_timeSamplingRate = m_newTimeSamplingRate;
		m_item->m_timeDivision = m_newTimeDivision;
		m_item->m_settings = m_newSettings;
		m_item->m_regionMode = m_newRegionMode;

		m_item->updateActorSettings();
		m_item->setIsCommandExecuting(false);
	}

private:
	bool m_oldEnabled;
	QString m_oldSolution;
	Post2dWindowNodeVectorParticleGroupDataItem::TimeMode m_oldTimeMode;
	int m_oldTimeSamplingRate;
	int m_oldTimeDivision;
	QList<Post2dWindowUnstructuredParticleSetSetting> m_oldSettings;
	StructuredGridRegion::RegionMode m_oldRegionMode;

	bool m_newEnabled;
	QString m_newSolution;
	Post2dWindowNodeVectorParticleGroupDataItem::TimeMode m_newTimeMode;
	int m_newTimeSamplingRate;
	int m_newTimeDivision;
	QList<Post2dWindowUnstructuredParticleSetSetting> m_newSettings;
	StructuredGridRegion::RegionMode m_newRegionMode;

	Post2dWindowNodeVectorParticleGroupUnstructuredDataItem* m_item;
};

void Post2dWindowParticleUnstructuredSettingDialog::accept()
{
	m_dataItem->clearSetting();
	iRICUndoStack::instance().push(new Post2dWindowParticleUnstructuredSetProperty(solution(), timeMode(), timeSamplingRate(), timeDivision(), m_settings, m_regionMode, m_dataItem));
	QDialog::accept();
}

void Post2dWindowParticleUnstructuredSettingDialog::reject()
{
	m_dataItem->clearSetting();
	m_dataItem->renderGraphicsView();
	QDialog::reject();
}

void Post2dWindowParticleUnstructuredSettingDialog::activeDataChanged(int index)
{
	if (index == -1 || index >= m_settings.count()){
		m_activeSetting = 0;
		return;
	}
	m_activeSetting = &(m_settings[index]);
	applySettings();
}

void Post2dWindowParticleUnstructuredSettingDialog::handleTimeSliderChange(int val)
{
	if (val < m_skipNominations.count()){
		ui->timeValueLabel->setText(QString("1/%1").arg(m_skipNominations.at(m_skipNominations.count() - val - 1)));
	} else if (val > m_skipNominations.count()){
		ui->timeValueLabel->setText(QString("%1").arg(m_subDivNominations.at(val - m_skipNominations.count() - 1)));
	} else {
		ui->timeValueLabel->setText("1");
	}
}

void Post2dWindowParticleUnstructuredSettingDialog::pointsEdited()
{
	if (m_applying){return;}
	if (ui->point1XEdit->text() == ""){return;}
	if (ui->point1YEdit->text() == ""){return;}
	if (ui->point2XEdit->text() == ""){return;}
	if (ui->point2YEdit->text() == ""){return;}
	if (m_activeSetting == 0){return;}
	m_activeSetting->point1 = QVector2D(ui->point1XEdit->value(), ui->point1YEdit->value());
	m_activeSetting->point2 = QVector2D(ui->point2XEdit->value(), ui->point2YEdit->value());

	m_activeSetting->pointsSet = true;
}

void Post2dWindowParticleUnstructuredSettingDialog::handleButtonPress(QAbstractButton* button)
{
	if (button == ui->buttonBox->button(QDialogButtonBox::Apply)){
		apply();
	}
}

void Post2dWindowParticleUnstructuredSettingDialog::numPointsChanged(int num)
{
	if (m_activeSetting == 0){return;}
	m_activeSetting->numberOfPoints = num;
}

void Post2dWindowParticleUnstructuredSettingDialog::colorChanged(const QColor &color)
{
	if (m_activeSetting == 0){return;}
	m_activeSetting->color = color;
}

void Post2dWindowParticleUnstructuredSettingDialog::sizeChanged(int size)
{
	if (m_activeSetting == 0){return;}
	m_activeSetting->size = size;
}

void Post2dWindowParticleUnstructuredSettingDialog::addData()
{
	if (m_activeSetting == 0){return;}
	Post2dWindowUnstructuredParticleSetSetting setting = *m_activeSetting;
	m_settings.append(setting);
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
	if (item != 0){delete item;}
	ui->startPositionListWidget->blockSignals(false);
	m_settings.removeAt(current);
	if (current >= m_settings.count()){current = m_settings.count() - 1;}
	ui->startPositionListWidget->setCurrentRow(current);
	m_activeSetting = &(m_settings[current]);
	applySettings();
	updateRemoveButtonStatus();
}

void Post2dWindowParticleUnstructuredSettingDialog::showRegionDialog()
{
	Post2dGridRegionSelectDialog dialog(this);
	if (! m_activeAvailable){
		dialog.disableActive();
	}
	dialog.hideCustom();
	dialog.setRegionMode(m_regionMode);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected){return;}
	m_regionMode = dialog.regionMode();
}

void Post2dWindowParticleUnstructuredSettingDialog::apply(){
	QVector2D v1(ui->point1XEdit->value(), ui->point1YEdit->value());
	QVector2D v2(ui->point2XEdit->value(), ui->point2YEdit->value());
	m_dataItem->setSetting(v1, v2, ui->numPointsSpinBox->value(), ui->sizeSpinBox->value());
}

void Post2dWindowParticleUnstructuredSettingDialog::setupNominations()
{
	int noms[] = {2, 3, 4, 5, 0};
	int idx = 0;
	while (noms[idx] != 0){
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
	int num = pd->GetNumberOfArrays();
	ui->solutionComboBox->blockSignals(true);
	for (int i = 0; i < num; ++i){
		vtkDataArray* da = pd->GetArray(i);
		if (da == 0){continue;}
		QString name = da->GetName();
		if (da->GetNumberOfComponents() <= 1){
			// scalar attributes.
			continue;
		}
		ui->solutionComboBox->addItem(gt->solutionCaption(name));
		m_solutions.append(name);
	}
	ui->solutionComboBox->blockSignals(false);
	if (m_solutions.count() <= 1){
		ui->physValLabel->hide();
		ui->solutionComboBox->hide();
	}
}

void Post2dWindowParticleUnstructuredSettingDialog::setupSettingList()
{
	for (int i = 0; i < m_settings.count(); ++i){
		ui->startPositionListWidget->addItem(QString("%1").arg(i + 1));
	}
	// select the first one.
	ui->startPositionListWidget->setCurrentRow(0);
	updateRemoveButtonStatus();
}

void Post2dWindowParticleUnstructuredSettingDialog::applySettings()
{
	if (m_activeSetting == 0){return;}
	m_applying = true;
	ui->pointsMouseRadioButton->setChecked(true);
	if (m_activeSetting->pointsSet){
		ui->point1XEdit->setValue(m_activeSetting->point1.x());
		ui->point1YEdit->setValue(m_activeSetting->point1.y());
		ui->point2XEdit->setValue(m_activeSetting->point2.x());
		ui->point2YEdit->setValue(m_activeSetting->point2.y());
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
	ui->removePushButton->setEnabled(m_settings.count() > 1);
}
