#include "../post2dwindowzonedataitem.h"
#include "post2dwindownodevectorstreamlinegroupstructureddataitem_impl.h"
#include "post2dwindownodevectorstreamlinegroupstructureddataitem_settingeditwidget.h"
#include "ui_post2dwindownodevectorstreamlinegroupstructureddataitem_settingeditwidget.h"

#include <guibase/comboboxtool.h>
#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/stringtool.h>
#include <misc/valuemodifycommandt.h>

Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::SettingEditWidget(Post2dWindowNodeVectorStreamlineGroupStructuredDataItem* item, QWidget *parent) :
	ModifyCommandWidget(parent),
	m_current {-1},
	m_item {item},
	ui(new Ui::Post2dWindowNodeVectorStreamlineGroupStructuredDataItem_SettingEditWidget)
{
	ui->setupUi(this);
	connect(ui->addButton, &QPushButton::clicked, this, &SettingEditWidget::addStartPosition);
	connect(ui->removeButton, &QPushButton::clicked, this, &SettingEditWidget::removeStartPosition);
	connect(ui->startPositionListWidget, &QListWidget::currentRowChanged, this, &SettingEditWidget::handleCurrentStartPositionChanged);

	auto cont = dynamic_cast<Post2dWindowZoneDataItem*>(m_item->parent())->dataContainer();
	auto grid = vtkStructuredGrid::SafeDownCast(cont->data()->data());
	int dims[3];
	grid->GetDimensions(dims);
	ui->regionWidget->setDimensions(dims[0], dims[1]);
	if (! cont->IBCExists()) {
		ui->regionWidget->disableActive();
	}
	ui->startPositionWidget->setDimensions(dims[0], dims[1]);
	setupSolutionComboBox(cont);

	setSetting(m_item->impl->m_setting);
}

Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

QUndoCommand* Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget"), apply);
	command->addCommand(new ValueModifyCommmand<Post2dWindowNodeVectorStreamlineGroupDataItem::Setting>(iRIC::generateCommandId("BaseSetting"),
																																																			apply, baseSetting(), &m_item->m_setting));
	command->addCommand(new ValueModifyCommmand<Setting>(iRIC::generateCommandId("StructuredSetting"),
																					apply, setting(), &m_item->impl->m_setting));
	return command;
}

Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::Setting Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::setting() const
{
	auto current = ui->startPositionListWidget->currentRow();

	auto startPositions = m_startPositions;
	startPositions[current] = ui->startPositionWidget->setting();

	Setting ret;

	ret.region = ui->regionWidget->setting();
	ret.startPositions = startPositions;

	return ret;
}
void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::setSetting(const Setting& setting)
{
	ui->regionWidget->setSetting(setting.region);

	m_startPositions = setting.startPositions;

	updateStartPositionList();
	ui->startPositionListWidget->setCurrentRow(0);
	applyStartPosition(0);
	updateRemoveButtonStatus();
}

Post2dWindowNodeVectorStreamlineGroupDataItem::Setting Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::baseSetting() const
{
	Post2dWindowNodeVectorStreamlineGroupDataItem::Setting ret;
	ret.target = m_solutions.at(ui->solutionComboBox->currentIndex()).c_str();

	return ret;
}

void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::setBaseSetting(const Post2dWindowNodeVectorStreamlineGroupDataItem::Setting& setting)
{
	auto it = std::find(m_solutions.begin(), m_solutions.end(), iRIC::toStr(setting.target));
	if (it != m_solutions.end()) {
		ui->solutionComboBox->setCurrentIndex(it - m_solutions.begin());
	}
}

void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::addStartPosition()
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

void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::removeStartPosition()
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

void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::handleCurrentStartPositionChanged(int index)
{
	if (m_current >= 0) {
		m_startPositions[m_current] = ui->startPositionWidget->setting();
	}
	applyStartPosition(index);
}

void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::setupSolutionComboBox(PostZoneDataContainer* zoneData)
{
	vtkPointData* pd = zoneData->data()->data()->GetPointData();
	SolverDefinitionGridType* gt = zoneData->gridType();

	m_solutions = vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(pd);
	ComboBoxTool::setupItems(gt->solutionCaptions(m_solutions), ui->solutionComboBox);

	if (m_solutions.size() < 2) {
		ui->physValLabel->hide();
		ui->solutionComboBox->hide();
	}
}

void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::updateRemoveButtonStatus()
{
	ui->removeButton->setEnabled(m_startPositions.size() > 1);
}

void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::updateStartPositionList()
{
	auto listWidget = ui->startPositionListWidget;
	listWidget->blockSignals(true);

	listWidget->clear();
	for (int i = 0; i < static_cast<int> (m_startPositions.size()); ++i) {
		listWidget->addItem(QString("%1").arg(i + 1));
	}

	listWidget->blockSignals(false);
}

void Post2dWindowNodeVectorStreamlineGroupStructuredDataItem::SettingEditWidget::applyStartPosition(int index)
{
	ui->startPositionWidget->setSetting(m_startPositions[index]);
	m_current = index;
}
