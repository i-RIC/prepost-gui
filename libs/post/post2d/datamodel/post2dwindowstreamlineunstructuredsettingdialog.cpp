#include "ui_post2dwindowstreamlineunstructuredsettingdialog.h"

#include "../post2dgridregionselectdialog.h"
#include "post2dwindownodevectorstreamlinegroupunstructureddataitem.h"
#include "post2dwindowstreamlineunstructuredsettingdialog.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>

#include <QPushButton>

#include <vtkPointData.h>
#include <vtkStructuredGrid.h>

Post2dWindowStreamlineUnstructuredSettingDialog::Post2dWindowStreamlineUnstructuredSettingDialog(QWidget* parent) :
	QDialog(parent),
	ui(new Ui::Post2dWindowStreamlineUnstructuredSettingDialog)
{
	setAttribute(Qt::WA_DeleteOnClose);
	ui->setupUi(this);

	connect(ui->startPositionListWidget, SIGNAL(currentRowChanged(int)), this, SLOT(activeDataChanged(int)));
	connect(ui->point1XEdit, SIGNAL(textChanged(QString)), this, SLOT(pointsEdited()));
	connect(ui->point1YEdit, SIGNAL(textChanged(QString)), this, SLOT(pointsEdited()));
	connect(ui->point2XEdit, SIGNAL(textChanged(QString)), this, SLOT(pointsEdited()));
	connect(ui->point2YEdit, SIGNAL(textChanged(QString)), this, SLOT(pointsEdited()));
	connect(ui->numPointsSpinBox, SIGNAL(valueChanged(int)), this, SLOT(numPointsChanged(int)));
	connect(ui->colorWidget, SIGNAL(colorChanged(QColor)), this, SLOT(colorChanged(QColor)));
	connect(ui->widthSpinBox, SIGNAL(valueChanged(int)), this, SLOT(widthChanged(int)));
	connect(ui->addPushButton, SIGNAL(clicked()), this, SLOT(addData()));
	connect(ui->removePushButton, SIGNAL(clicked()), this, SLOT(removeData()));
	connect(ui->regionSettingButton, SIGNAL(clicked()), this, SLOT(showRegionDialog()));

	connect(ui->buttonBox, SIGNAL(clicked(QAbstractButton*)), this, SLOT(handleButtonPress(QAbstractButton*)));

	m_pressed = false;
	m_applying = false;
}

Post2dWindowStreamlineUnstructuredSettingDialog::~Post2dWindowStreamlineUnstructuredSettingDialog()
{
	delete ui;
}

void Post2dWindowStreamlineUnstructuredSettingDialog::setZoneData(PostZoneDataContainer* zoneData)
{
	setupSolutionComboBox(zoneData);
}

void Post2dWindowStreamlineUnstructuredSettingDialog::setSolution(const QString& sol)
{
	int index = m_solutions.indexOf(sol);
	if (index == -1) {index = 0;}
	ui->solutionComboBox->setCurrentIndex(index);
}

QString Post2dWindowStreamlineUnstructuredSettingDialog::solution() const
{
	int index = ui->solutionComboBox->currentIndex();
	return m_solutions.at(index);
}

void Post2dWindowStreamlineUnstructuredSettingDialog::informButtonDown(const QVector2D& p)
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

void Post2dWindowStreamlineUnstructuredSettingDialog::informButtonUp(const QVector2D& p)
{
	if (ui->pointsMouseRadioButton->isChecked()) {
		ui->point2XEdit->setValue(p.x());
		ui->point2YEdit->setValue(p.y());
	}
	m_pressed = false;
	apply();
}

void Post2dWindowStreamlineUnstructuredSettingDialog::updateMousePosition(const QVector2D& p)
{
	if (ui->pointsMouseRadioButton->isChecked() && m_pressed) {
		ui->point2XEdit->setValue(p.x());
		ui->point2YEdit->setValue(p.y());
		apply();
	}
}

class Post2dWindowStreamlineUnstructuredSetProperty : public QUndoCommand
{
public:
	Post2dWindowStreamlineUnstructuredSetProperty(const QString& sol, const QList<Post2dWindowUnstructuredStreamlineSetSetting>& settings, StructuredGridRegion::RegionMode rm, Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem* item)
		: QUndoCommand(QObject::tr("Update Streamline Setting")) {
		m_newEnabled = true;
		m_newSolution = sol;
		m_newSettings = settings;
		m_newRegionMode = rm;

		m_oldEnabled = item->isEnabled();
		m_oldSolution = item->m_currentSolution;
		m_oldSettings = item->m_settings;
		m_oldRegionMode = item->m_regionMode;

		m_item = item;
	}
	void undo() {
		m_item->setIsCommandExecuting(true);

		m_item->setEnabled(m_oldEnabled);
		m_item->setCurrentSolution(m_oldSolution);
		m_item->m_settings = m_oldSettings;
		m_item->m_regionMode = m_oldRegionMode;

		m_item->informGridUpdate();
		m_item->setIsCommandExecuting(false);
	}
	void redo() {
		m_item->setIsCommandExecuting(true);

		m_item->setEnabled(m_newEnabled);
		m_item->setCurrentSolution(m_newSolution);
		m_item->m_settings = m_newSettings;
		m_item->m_regionMode = m_newRegionMode;

		m_item->informGridUpdate();
		m_item->setIsCommandExecuting(false);
	}
private:
	bool m_newEnabled;
	QString m_newSolution;
	QList<Post2dWindowUnstructuredStreamlineSetSetting> m_newSettings;
	StructuredGridRegion::RegionMode m_newRegionMode;

	bool m_oldEnabled;
	QString m_oldSolution;
	QList<Post2dWindowUnstructuredStreamlineSetSetting> m_oldSettings;
	StructuredGridRegion::RegionMode m_oldRegionMode;

	Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem* m_item;
};

void Post2dWindowStreamlineUnstructuredSettingDialog::accept()
{
	m_dataItem->clearSetting();
	iRICUndoStack::instance().push(new Post2dWindowStreamlineUnstructuredSetProperty(solution(), settings(), regionMode(), m_dataItem));
	QDialog::accept();
}

void Post2dWindowStreamlineUnstructuredSettingDialog::reject()
{
	m_dataItem->clearSetting();
	m_dataItem->renderGraphicsView();
	QDialog::reject();
}

void Post2dWindowStreamlineUnstructuredSettingDialog::activeDataChanged(int index)
{
	if (index == -1 || index >= m_settings.count()) {
		m_activeSetting = nullptr;
		return;
	}
	m_activeSetting = &(m_settings[index]);
	applySettings();
}

void Post2dWindowStreamlineUnstructuredSettingDialog::pointsEdited()
{
	if (m_applying) {return;}
	if (ui->point1XEdit->text() == "") {return;}
	if (ui->point1YEdit->text() == "") {return;}
	if (ui->point2XEdit->text() == "") {return;}
	if (ui->point2YEdit->text() == "") {return;}
	if (m_activeSetting == nullptr) {return;}
	m_activeSetting->point1 = QVector2D(ui->point1XEdit->value(), ui->point1YEdit->value());
	m_activeSetting->point2 = QVector2D(ui->point2XEdit->value(), ui->point2YEdit->value());

	m_activeSetting->pointsSet = true;
}

void Post2dWindowStreamlineUnstructuredSettingDialog::handleButtonPress(QAbstractButton* button)
{
	if (button == ui->buttonBox->button(QDialogButtonBox::Apply)) {
		apply();
	}
}

void Post2dWindowStreamlineUnstructuredSettingDialog::numPointsChanged(int num)
{
	if (m_activeSetting == nullptr) {return;}
	m_activeSetting->numberOfPoints = num;
}

void Post2dWindowStreamlineUnstructuredSettingDialog::colorChanged(const QColor& color)
{
	if (m_activeSetting == nullptr) {return;}
	m_activeSetting->color = color;
}

void Post2dWindowStreamlineUnstructuredSettingDialog::widthChanged(int width)
{
	if (m_activeSetting == nullptr) {return;}
	m_activeSetting->width = width;
}

void Post2dWindowStreamlineUnstructuredSettingDialog::addData()
{
	if (m_activeSetting == nullptr) {return;}
	Post2dWindowUnstructuredStreamlineSetSetting setting = *m_activeSetting;
	m_settings.append(setting);
	QListWidgetItem* tmpitem = ui->startPositionListWidget->item(ui->startPositionListWidget->count() - 1);
	int tmpint = tmpitem->text().toInt();
	++ tmpint;
	ui->startPositionListWidget->addItem(QString("%1").arg(tmpint));
	ui->startPositionListWidget->setCurrentRow(ui->startPositionListWidget->count() - 1);
	updateRemoveButtonStatus();
}

void Post2dWindowStreamlineUnstructuredSettingDialog::removeData()
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

void Post2dWindowStreamlineUnstructuredSettingDialog::showRegionDialog()
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

void Post2dWindowStreamlineUnstructuredSettingDialog::apply()
{
	QVector2D v1(ui->point1XEdit->value(), ui->point1YEdit->value());
	QVector2D v2(ui->point2XEdit->value(), ui->point2YEdit->value());
	m_dataItem->setSetting(v1, v2, ui->numPointsSpinBox->value());
}

void Post2dWindowStreamlineUnstructuredSettingDialog::setupSolutionComboBox(PostZoneDataContainer* zoneData)
{
	vtkPointData* pd = zoneData->data()->GetPointData();
	SolverDefinitionGridType* gt = zoneData->gridType();
	int num = pd->GetNumberOfArrays();
	ui->solutionComboBox->blockSignals(true);
	for (int i = 0; i < num; ++i) {
		vtkDataArray* da = pd->GetArray(i);
		if (da == nullptr) {continue;}
		QString name = da->GetName();
		if (da->GetNumberOfComponents() <= 1) {
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

void Post2dWindowStreamlineUnstructuredSettingDialog::setupSettingList()
{
	for (int i = 0; i < m_settings.count(); ++i) {
		ui->startPositionListWidget->addItem(QString("%1").arg(i + 1));
	}
	// select the first one.
	ui->startPositionListWidget->setCurrentRow(0);
	updateRemoveButtonStatus();
}

void Post2dWindowStreamlineUnstructuredSettingDialog::applySettings()
{
	if (m_activeSetting == nullptr) {return;}
	m_applying = true;
	ui->pointsMouseRadioButton->setChecked(true);
	if (m_activeSetting->pointsSet) {
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
	ui->widthSpinBox->setValue(m_activeSetting->width);
	m_applying = false;
	apply();
}

void Post2dWindowStreamlineUnstructuredSettingDialog::updateRemoveButtonStatus()
{
	ui->removePushButton->setEnabled(m_settings.count() > 1);
}
