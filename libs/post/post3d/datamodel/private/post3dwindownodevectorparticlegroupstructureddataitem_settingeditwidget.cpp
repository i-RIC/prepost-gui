#include "../post3dwindowzonedataitem.h"
#include "post3dwindownodevectorparticlegroupstructureddataitem_impl.h"
#include "post3dwindownodevectorparticlegroupstructureddataitem_settingeditwidget.h"
#include "ui_post3dwindownodevectorparticlegroupstructureddataitem_settingeditwidget.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

Post3dWindowNodeVectorParticleGroupStructuredDataItem::SettingEditWidget::SettingEditWidget(Post3dWindowNodeVectorParticleGroupStructuredDataItem* item, QWidget *parent) :
	ModifyCommandWidget(parent),
	m_current {-1},
	m_item {item},
	ui(new Ui::Post3dWindowNodeVectorParticleGroupStructuredDataItem_SettingEditWidget)
{
	ui->setupUi(this);

	connect(ui->addButton, &QPushButton::clicked, this, &SettingEditWidget::addStartPosition);
	connect(ui->removeButton, &QPushButton::clicked, this, &SettingEditWidget::removeStartPosition);
	connect(ui->startPositionListWidget, &QListWidget::currentRowChanged, this, &SettingEditWidget::handleCurrentStartPositionChanged);

	ui->particleSettingWidget->setProjectMainFile(m_item->projectData()->mainfile());

	auto cont = dynamic_cast<Post3dWindowZoneDataItem*>(m_item->parent())->dataContainer();
	auto grid = vtkStructuredGrid::SafeDownCast(cont->data()->data());

	auto gtype = cont->gridType();
	auto pd = grid->GetPointData();

	std::unordered_map<std::string, QString> solutions;
	for (const auto& sol : vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(pd)) {
		auto cap = gtype->outputCaption(sol);
		solutions.insert({sol, cap});
	}
	ui->particleSettingWidget->setSolutions(solutions);
	ui->particleSettingWidget->setSetting(&m_item->m_setting);

	int dims[3];
	grid->GetDimensions(dims);
	ui->startPositionWidget->setDimensions(dims);

	setSetting(m_item->impl->m_setting);
}

Post3dWindowNodeVectorParticleGroupStructuredDataItem::SettingEditWidget::~SettingEditWidget()
{
	delete ui;
}

QUndoCommand* Post3dWindowNodeVectorParticleGroupStructuredDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("Post3dWindowNodeVectorParticleGroupStructuredDataItem::ModifySetting"), apply);
	command->addCommand(ui->particleSettingWidget->createModifyCommand(apply));
	command->addCommand(new ValueModifyCommmand<Setting>(iRIC::generateCommandId("Post3dWindowNodeVectorParticleGroupStructuredDataItem::StartPositions"),
																					apply, setting(), &m_item->impl->m_setting));
	return command;
}

Post3dWindowNodeVectorParticleGroupStructuredDataItem::Setting Post3dWindowNodeVectorParticleGroupStructuredDataItem::SettingEditWidget::setting()
{
	auto current = ui->startPositionListWidget->currentRow();
	m_startPositions[current] = ui->startPositionWidget->setting();

	Setting ret;

	ret.startPositions = m_startPositions;

	return ret;
}
void Post3dWindowNodeVectorParticleGroupStructuredDataItem::SettingEditWidget::setSetting(const Setting& setting)
{
	m_startPositions = setting.startPositions;

	updateStartPositionList();
	ui->startPositionListWidget->setCurrentRow(0);
	applyStartPosition(0);
	updateRemoveButtonStatus();
}

void Post3dWindowNodeVectorParticleGroupStructuredDataItem::SettingEditWidget::addStartPosition()
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

void Post3dWindowNodeVectorParticleGroupStructuredDataItem::SettingEditWidget::removeStartPosition()
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

void Post3dWindowNodeVectorParticleGroupStructuredDataItem::SettingEditWidget::handleCurrentStartPositionChanged(int index)
{
	if (m_current >= 0) {
		m_startPositions[m_current] = ui->startPositionWidget->setting();
	}
	applyStartPosition(index);
}

void Post3dWindowNodeVectorParticleGroupStructuredDataItem::SettingEditWidget::updateRemoveButtonStatus()
{
	ui->removeButton->setEnabled(m_startPositions.size() > 1);
}

void Post3dWindowNodeVectorParticleGroupStructuredDataItem::SettingEditWidget::updateStartPositionList()
{
	auto listWidget = ui->startPositionListWidget;
	listWidget->blockSignals(true);

	listWidget->clear();
	for (int i = 0; i < static_cast<int> (m_startPositions.size()); ++i) {
		listWidget->addItem(QString("%1").arg(i + 1));
	}

	listWidget->blockSignals(false);
}

void Post3dWindowNodeVectorParticleGroupStructuredDataItem::SettingEditWidget::applyStartPosition(int index)
{
	ui->startPositionWidget->setSetting(m_startPositions[index]);
	m_current = index;
}
