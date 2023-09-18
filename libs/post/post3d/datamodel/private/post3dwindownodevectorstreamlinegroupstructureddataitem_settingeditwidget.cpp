#include "../post3dwindowzonedataitem.h"
#include "post3dwindownodevectorstreamlinegroupstructureddataitem_impl.h"
#include "post3dwindownodevectorstreamlinegroupstructureddataitem_settingeditwidget.h"
#include "ui_post3dwindownodevectorstreamlinegroupstructureddataitem_settingeditwidget.h"

#include <guibase/comboboxtool.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/grid/v4structured3dgrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::SettingEditWidget(Post3dWindowNodeVectorStreamlineGroupStructuredDataItem* item, QWidget *parent) :
	ModifyCommandWidget(parent),
	m_current {-1},
	m_item {item},
	ui(new Ui::Post3dWindowNodeVectorStreamlineGroupStructuredDataItem_SettingEditWidget)
{
	ui->setupUi(this);
	connect(ui->addButton, &QPushButton::clicked, this, &SettingEditWidget::addStartPosition);
	connect(ui->removeButton, &QPushButton::clicked, this, &SettingEditWidget::removeStartPosition);
	connect(ui->startPositionListWidget, &QListWidget::currentRowChanged, this, &SettingEditWidget::handleCurrentStartPositionChanged);

	auto cont = m_item->zoneDataItem()->v4DataContainer();
	auto grid = dynamic_cast<v4Structured3dGrid*> (cont->gridData()->grid())->vtkConcreteData()->concreteData();
	int dims[3];
	grid->GetDimensions(dims);
	ui->startPositionWidget->setDimensions(dims);
	setupSolutionComboBox(cont);

	setSetting(m_item->impl->m_setting);
}

Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

QUndoCommand* Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	return new ValueModifyCommmand<Setting>(iRIC::generateCommandId("Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::ModifySetting"),
																					apply, setting(), &m_item->impl->m_setting);
}

Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::setting()
{
	auto current = ui->startPositionListWidget->currentRow();
	m_startPositions[current] = ui->startPositionWidget->setting();

	Setting ret;

	ret.startPositions = m_startPositions;

	return ret;
}
void Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::setSetting(const Setting& setting)
{
	m_startPositions = setting.startPositions;

	updateStartPositionList();
	ui->startPositionListWidget->setCurrentRow(0);
	applyStartPosition(0);
	updateRemoveButtonStatus();
}

void Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::addStartPosition()
{
	auto current = ui->startPositionListWidget->currentRow();
	m_startPositions[current] = ui->startPositionWidget->setting();
	auto setting = m_startPositions[current];

	m_startPositions.push_back(setting);

	updateStartPositionList();
	int newIndex = static_cast<int>(m_startPositions.size() - 1);
	ui->startPositionListWidget->setCurrentRow(newIndex);
	applyStartPosition(newIndex);
	updateRemoveButtonStatus();
}

void Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::removeStartPosition()
{
	auto listWidget = ui->startPositionListWidget;
	int current = listWidget->currentRow();
	auto it = m_startPositions.begin() + current;
	m_startPositions.erase(it);

	if (current >= static_cast<int>(m_startPositions.size())) {current = static_cast<int> (m_startPositions.size()) - 1;}

	updateStartPositionList();

	listWidget->blockSignals(true);
	ui->startPositionListWidget->setCurrentRow(current);
	listWidget->blockSignals(false);

	applyStartPosition(current);
	updateRemoveButtonStatus();
}

void Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::handleCurrentStartPositionChanged(int index)
{
	if (m_current >= 0) {
		m_startPositions[m_current] = ui->startPositionWidget->setting();
	}
	applyStartPosition(index);
}

void Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::setupSolutionComboBox(v4PostZoneDataContainer* zoneData)
{
	vtkPointData* pd = zoneData->gridData()->grid()->vtkData()->data()->GetPointData();
	SolverDefinitionGridType* gt = zoneData->gridType();

	m_solutions = vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(pd);
	ComboBoxTool::setupItems(gt->outputCaptions(m_solutions), ui->solutionComboBox);

	if (m_solutions.size() < 2) {
		m_item->m_setting.target = m_solutions[0].c_str();

		ui->physValLabel->hide();
		ui->solutionComboBox->hide();
	}
}

void Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::updateRemoveButtonStatus()
{
	ui->removeButton->setEnabled(m_startPositions.size() > 1);
}

void Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::updateStartPositionList()
{
	auto listWidget = ui->startPositionListWidget;
	listWidget->blockSignals(true);

	listWidget->clear();
	for (int i = 0; i < static_cast<int> (m_startPositions.size()); ++i) {
		listWidget->addItem(QString("%1").arg(i + 1));
	}

	listWidget->blockSignals(false);
}

void Post3dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::applyStartPosition(int index)
{
	ui->startPositionWidget->setSetting(m_startPositions[index]);
	m_current = index;
}
