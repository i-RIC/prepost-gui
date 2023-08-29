#include "../post2dwindowzonedataitem.h"
#include "post2dwindownodevectorparticlegroupunstructureddataitem_impl.h"
#include "post2dwindownodevectorparticlegroupunstructureddataitem_settingeditwidget.h"
#include "ui_post2dwindownodevectorparticlegroupunstructureddataitem_settingeditwidget.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/mergesupportedlistcommand.h>
#include <misc/qundocommandhelper.h>
#include <misc/valuemodifycommandt.h>

Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::SettingEditWidget::SettingEditWidget(Post2dWindowNodeVectorParticleGroupUnstructuredDataItem* item, QWidget *parent) :
	ModifyCommandWidget(parent),
	m_current {-1},
	m_item {item},
	ui(new Ui::Post2dWindowNodeVectorParticleGroupUnstructuredDataItem_SettingEditWidget)
{
	ui->setupUi(this);
	connect(ui->addButton, &QPushButton::clicked, this, &SettingEditWidget::addStartPosition);
	connect(ui->removeButton, &QPushButton::clicked, this, &SettingEditWidget::removeStartPosition);
	connect(ui->startPositionListWidget, &QListWidget::currentRowChanged, this, &SettingEditWidget::handleCurrentStartPositionChanged);

	ui->regionWidget->hideCustom();

	ui->particleSettingWidget->setProjectMainFile(m_item->projectData()->mainfile());

	auto cont = dynamic_cast<Post2dWindowZoneDataItem*>(m_item->parent())->dataContainer();
	auto grid = cont->data()->data();

	auto gtype = cont->gridType();
	auto pd = grid->GetPointData();

	std::unordered_map<std::string, QString> solutions;
	for (const auto& sol : vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(pd)) {
		auto cap = gtype->outputCaption(sol);
		solutions.insert({sol, cap});
	}
	ui->particleSettingWidget->setSolutions(solutions);
	ui->particleSettingWidget->setSetting(&m_item->m_setting);

	if (! cont->IBCExists()) {
		ui->regionWidget->disableActive();
	}

	setSetting(m_item->impl->m_setting);
}

Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::SettingEditWidget::~SettingEditWidget()
{
	m_item->hidePreviewSetting();
	m_item->renderGraphicsView();

	delete ui;
}

QUndoCommand* Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::SettingEditWidget::createModifyCommand(bool apply)
{
	auto s = ui->startPositionWidget->setting();
	m_item->updatePreview(s.point1, s.point2, s.numberOfPoints);

	auto command = new MergeSupportedListCommand(iRIC::generateCommandId("Post2dWindowNodeVectorParticleGroupStructuredDataItem::ModifySetting"), apply);
	command->addCommand(ui->particleSettingWidget->createModifyCommand(apply));
	command->addCommand(new ValueModifyCommmand<Setting>(iRIC::generateCommandId("Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::ModifySetting"),
																											 apply, setting(), &m_item->impl->m_setting));
	return command;
}

Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::SettingEditWidget::setting()
{
	auto current = ui->startPositionListWidget->currentRow();
	m_startPositions[current] = ui->startPositionWidget->setting();

	Setting ret;

	ret.region = ui->regionWidget->setting();
	ret.startPositions = m_startPositions;

	return ret;
}
void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::SettingEditWidget::setSetting(const Setting& setting)
{
	ui->regionWidget->setSetting(setting.region);

	m_startPositions = setting.startPositions;

	updateStartPositionList();
	ui->startPositionListWidget->setCurrentRow(0);
	applyStartPosition(0);
	updateRemoveButtonStatus();
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::SettingEditWidget::addStartPosition()
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

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::SettingEditWidget::removeStartPosition()
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

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::SettingEditWidget::handleCurrentStartPositionChanged(int index)
{
	if (m_current >= 0) {
		m_startPositions[m_current] = ui->startPositionWidget->setting();
	}
	applyStartPosition(index);
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::SettingEditWidget::handleMousePoint1Change(const QPointF point)
{
	if (! ui->startPositionWidget->handleMousePoint1Change(point)) {return;}
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::SettingEditWidget::handleMousePoint2Change(const QPointF point)
{
	if (! ui->startPositionWidget->handleMousePoint2Change(point)) {return;}

	auto s = ui->startPositionWidget->setting();
	m_item->updatePreview(s.point1, s.point2, s.numberOfPoints);

	m_item->renderGraphicsView();
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::SettingEditWidget::updateRemoveButtonStatus()
{
	ui->removeButton->setEnabled(m_startPositions.size() > 1);
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::SettingEditWidget::updateStartPositionList()
{
	auto listWidget = ui->startPositionListWidget;
	listWidget->blockSignals(true);

	listWidget->clear();
	for (int i = 0; i < static_cast<int> (m_startPositions.size()); ++i) {
		listWidget->addItem(QString("%1").arg(i + 1));
	}

	listWidget->blockSignals(false);
}

void Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::SettingEditWidget::applyStartPosition(int index)
{
	ui->startPositionWidget->setSetting(m_startPositions[index]);
	m_current = index;
}
