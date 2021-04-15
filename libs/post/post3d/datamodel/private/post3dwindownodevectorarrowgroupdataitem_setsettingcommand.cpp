#include "post3dwindownodevectorarrowgroupdataitem_setsettingcommand.h"
#include "../post3dwindownodevectorarrowdataitem.h"

Post3dWindowNodeVectorArrowGroupDataItem::SetSettingCommand::SetSettingCommand(const ArrowSettingContainer& arrowSetting, const std::vector<Post3dWindowNodeVectorArrowSettingDialog::FaceSetting>& faceSettings, Post3dWindowNodeVectorArrowGroupDataItem* item) :
	m_newArrowSetting {arrowSetting},
	m_newFaceSettings (faceSettings),
	m_oldArrowSetting {item->m_setting},
	m_item {item}
{
	for (auto c : item->m_childItems) {
		auto c2 = dynamic_cast<Post3dWindowNodeVectorArrowDataItem*> (c);
		m_oldFaceSettings.push_back(c2->setting());
	}
}

Post3dWindowNodeVectorArrowGroupDataItem::SetSettingCommand::~SetSettingCommand()
{}

void Post3dWindowNodeVectorArrowGroupDataItem::SetSettingCommand::redo()
{
	applySetting(m_newArrowSetting, m_newFaceSettings);
}

void Post3dWindowNodeVectorArrowGroupDataItem::SetSettingCommand::undo()
{
	applySetting(m_oldArrowSetting, m_oldFaceSettings);
}

void Post3dWindowNodeVectorArrowGroupDataItem::SetSettingCommand::applySetting(const ArrowSettingContainer& arrowSetting, const std::vector<Post3dWindowNodeVectorArrowSettingDialog::FaceSetting>& faceSettings)
{
	m_item->setSetting(arrowSetting, faceSettings);
}
