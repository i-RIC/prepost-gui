#include "post3dwindowcontourgroupdataitem_setfacescommand.h"

#include <misc/qundocommandhelper.h>

Post3dWindowContourGroupDataItem::SetFacesCommand::SetFacesCommand(const std::vector<Post3dWindowFaceSettingContainer>& faces, Post3dWindowContourGroupDataItem* item) :
	QUndoCommand("Update faces"),
	m_newFaces {faces},
	m_oldFaces {item->faces()},
	m_item {item}
{}

void Post3dWindowContourGroupDataItem::SetFacesCommand::redo()
{
	apply(m_newFaces);
}

void Post3dWindowContourGroupDataItem::SetFacesCommand::undo()
{
	apply(m_oldFaces);
}

int Post3dWindowContourGroupDataItem::SetFacesCommand::id() const
{
	return iRIC::generateCommandId("Post3dWindowContourGroupDataItem::SetFacesCommand");
}

bool Post3dWindowContourGroupDataItem::SetFacesCommand::mergeWith(const QUndoCommand* other)
{
	auto other2 = dynamic_cast<const SetFacesCommand*> (other);
	if (other2 == nullptr) {return false;}

	if (m_item != other2->m_item) {return false;}
	m_newFaces = other2->m_newFaces;

	return true;
}

void Post3dWindowContourGroupDataItem::SetFacesCommand::apply(const std::vector<Post3dWindowFaceSettingContainer>& ranges)
{
	m_item->setFaces(ranges);
}
