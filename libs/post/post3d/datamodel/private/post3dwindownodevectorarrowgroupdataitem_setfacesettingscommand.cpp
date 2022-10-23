#include "post3dwindownodevectorarrowgroupdataitem_setfacesettingscommand.h"

#include <misc/qundocommandhelper.h>

Post3dWindowNodeVectorArrowGroupDataItem::SetFaceSettingsCommand::SetFaceSettingsCommand(const std::vector<Post3dWindowNodeVectorArrowDataItem::Setting>& settings, Post3dWindowNodeVectorArrowGroupDataItem* item) :
	m_newSettings {settings},
	m_oldSettings {item->faceSettings()},
	m_item {item}
{}

void Post3dWindowNodeVectorArrowGroupDataItem::SetFaceSettingsCommand::redo()
{
	apply(m_newSettings);
}

void Post3dWindowNodeVectorArrowGroupDataItem::SetFaceSettingsCommand::undo()
{
	apply(m_oldSettings);
}

int Post3dWindowNodeVectorArrowGroupDataItem::SetFaceSettingsCommand::id() const
{
	return iRIC::generateCommandId("Post3dWindowNodeVectorArrowGroupDataItem::SetFaceSettingsCommand");
}

bool Post3dWindowNodeVectorArrowGroupDataItem::SetFaceSettingsCommand::mergeWith(const QUndoCommand* other)
{
	auto other2 = dynamic_cast<const SetFaceSettingsCommand*> (other);
	if (other2 == nullptr) {return false;}

	if (m_item != other2->m_item) {return false;}
	m_newSettings = other2->m_newSettings;

	return true;
}

void Post3dWindowNodeVectorArrowGroupDataItem::SetFaceSettingsCommand::apply(const std::vector<Post3dWindowNodeVectorArrowDataItem::Setting>& settings)
{
	m_item->setFaceSettings(settings);
}
