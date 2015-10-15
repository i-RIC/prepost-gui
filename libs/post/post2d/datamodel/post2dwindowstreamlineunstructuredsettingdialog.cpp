#include "ui_post2dwindowstreamlineunstructuredsettingdialog.h"

#include "../post2dgridregionselectdialog.h"
#include "post2dwindownodevectorstreamlinegroupunstructureddataitem.h"
#include "post2dwindowstreamlineunstructuredsettingdialog.h"

#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>

#include <QPushButton>
#include <QVector2D>

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

void Post2dWindowStreamlineUnstructuredSettingDialog::setSettings(const Post2dWindowNodeVectorStreamlineGroupDataItem::Setting& s, const std::vector<Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Setting>& unsts)
{
	m_setting = s;
	m_unstSettings = unsts;

	int index = m_solutions.indexOf(s.currentSolution);
	if (index == -1) {index = 0;}
	ui->solutionComboBox->setCurrentIndex(index);

	setupSettingList();
}

Post2dWindowNodeVectorStreamlineGroupDataItem::Setting Post2dWindowStreamlineUnstructuredSettingDialog::setting() const
{
	Post2dWindowNodeVectorStreamlineGroupDataItem::Setting ret = m_setting;

	// solution
	int index = ui->solutionComboBox->currentIndex();
	ret.currentSolution = m_solutions.at(index);

	return ret;
}

void Post2dWindowStreamlineUnstructuredSettingDialog::informButtonDown(const QPointF& p)
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

void Post2dWindowStreamlineUnstructuredSettingDialog::informButtonUp(const QPointF& p)
{
	if (ui->pointsMouseRadioButton->isChecked()) {
		ui->point2XEdit->setValue(p.x());
		ui->point2YEdit->setValue(p.y());
	}
	m_pressed = false;
	apply();
}

void Post2dWindowStreamlineUnstructuredSettingDialog::updateMousePosition(const QVector2D &p)
{
	if (ui->pointsMouseRadioButton->isChecked() && m_pressed) {
		ui->point2XEdit->setValue(p.x());
		ui->point2YEdit->setValue(p.y());
		apply();
	}
}

class Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::SetSettingCommand  : public QUndoCommand
{
public:
	SetSettingCommand(const Post2dWindowNodeVectorStreamlineGroupDataItem::Setting& s, const std::vector<Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Setting>& settings, Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem* item) :
		QUndoCommand(Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::tr("Update Streamline Setting")),
		m_newSetting {s},
		m_newUnstSettings (settings),
		m_oldEnabled {item->isEnabled()},
		m_oldSetting {item->m_setting},
		m_oldUnstSettings (item->m_unstSettings),
		m_item {item}
	{}
	void redo() {
		m_item->setEnabled(true);
		m_item->m_setting = m_newSetting;
		m_item->setCurrentSolution(m_newSetting.currentSolution);
		m_item->m_unstSettings = m_newUnstSettings;

		m_item->updateActorSettings();
	}
	void undo() {
		m_item->setEnabled(m_oldEnabled);
		m_item->m_setting = m_oldSetting;
		m_item->setCurrentSolution(m_oldSetting.currentSolution);
		m_item->m_unstSettings = m_oldUnstSettings;

		m_item->updateActorSettings();
	}

private:
	Post2dWindowNodeVectorStreamlineGroupDataItem::Setting m_newSetting;
	std::vector<Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Setting> m_newUnstSettings;

	bool m_oldEnabled;
	Post2dWindowNodeVectorStreamlineGroupDataItem::Setting m_oldSetting;
	std::vector<Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Setting> m_oldUnstSettings;

	Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem* m_item;
};

void Post2dWindowStreamlineUnstructuredSettingDialog::accept()
{
	m_dataItem->hidePreviewSetting();
	m_dataItem->pushRenderCommand(new Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::SetSettingCommand(setting(), settings(), m_dataItem), m_dataItem, true);
	QDialog::accept();
}

void Post2dWindowStreamlineUnstructuredSettingDialog::reject()
{
	m_dataItem->hidePreviewSetting();
	m_dataItem->renderGraphicsView();
	QDialog::reject();
}

void Post2dWindowStreamlineUnstructuredSettingDialog::activeDataChanged(int index)
{
	if (index == -1 || index >= m_unstSettings.size()) {
		m_activeSetting = nullptr;
		return;
	}
	m_activeSetting = &(m_unstSettings[index]);
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
	m_activeSetting->point1 = QPointF(ui->point1XEdit->value(), ui->point1YEdit->value());
	m_activeSetting->point2 = QPointF(ui->point2XEdit->value(), ui->point2YEdit->value());

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
	Post2dWindowNodeVectorStreamlineGroupUnstructuredDataItem::Setting setting = *m_activeSetting;
	m_unstSettings.push_back(setting);
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
	m_unstSettings.erase(m_unstSettings.begin() + current);
	if (current >= m_unstSettings.size()) { current = m_unstSettings.size() - 1; }
	ui->startPositionListWidget->setCurrentRow(current);
	m_activeSetting = &(m_unstSettings[current]);
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
	dialog.setRegionMode(m_setting.regionMode);

	int ret = dialog.exec();
	if (ret == QDialog::Rejected) {return;}
	m_setting.regionMode = dialog.regionMode();
}

void Post2dWindowStreamlineUnstructuredSettingDialog::apply()
{
	QPointF v1(ui->point1XEdit->value(), ui->point1YEdit->value());
	QPointF v2(ui->point2XEdit->value(), ui->point2YEdit->value());
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
	for (int i = 0; i < m_unstSettings.size(); ++i) {
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
	ui->widthSpinBox->setValue(m_activeSetting->width);
	m_applying = false;
	apply();
}

void Post2dWindowStreamlineUnstructuredSettingDialog::updateRemoveButtonStatus()
{
	ui->removePushButton->setEnabled(m_unstSettings.size() > 1);
}
