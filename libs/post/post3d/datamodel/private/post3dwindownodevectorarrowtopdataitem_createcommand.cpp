#include "post3dwindownodevectorarrowtopdataitem_createcommand.h"

#include <QStandardItem>

Post3dWindowNodeVectorArrowTopDataItem::CreateCommand::CreateCommand(const ArrowSettingContainer& arrowSetting, const std::vector<Post3dWindowNodeVectorArrowSettingDialog::FaceSetting>& faceSettings, Post3dWindowNodeVectorArrowTopDataItem* item) :
	m_arrowSetting {arrowSetting},
	m_faceSettings (faceSettings),
	m_newItem {nullptr},
	m_item {item}
{}

Post3dWindowNodeVectorArrowTopDataItem::CreateCommand::~CreateCommand()
{}

void Post3dWindowNodeVectorArrowTopDataItem::CreateCommand::redo()
{
	m_newItem = new Post3dWindowNodeVectorArrowGroupDataItem(m_item);
	m_newItem->setIsCommandExecuting(true);

	m_newItem->standardItem()->setText(m_arrowSetting.target);
	m_newItem->updateZScale(m_item->m_zScale);
	m_newItem->setSetting(m_arrowSetting, m_faceSettings);
	m_item->m_childItems.push_back(m_newItem);

	m_newItem->setIsCommandExecuting(false);
	m_item->updateItemMap();
}
void Post3dWindowNodeVectorArrowTopDataItem::CreateCommand::undo()
{
	delete m_newItem;
	m_newItem = nullptr;

	m_item->update();
	m_item->updateItemMap();
}
