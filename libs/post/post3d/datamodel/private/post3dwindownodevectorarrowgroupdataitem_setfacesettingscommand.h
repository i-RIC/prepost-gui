#ifndef POST3DWINDOWNODEVECTORARROWGROUPDATAITEM_SETFACESETTINGSCOMMAND_H
#define POST3DWINDOWNODEVECTORARROWGROUPDATAITEM_SETFACESETTINGSCOMMAND_H

#include "../post3dwindownodevectorarrowgroupdataitem.h"

#include <QUndoCommand>

class Post3dWindowNodeVectorArrowGroupDataItem::SetFaceSettingsCommand : public QUndoCommand
{
public:
	SetFaceSettingsCommand(const std::vector<Post3dWindowNodeVectorArrowDataItem::Setting>& settings, Post3dWindowNodeVectorArrowGroupDataItem* item);

	void redo() override;
	void undo() override;

	int id() const override;
	bool mergeWith(const QUndoCommand* other) override;

private:
	void apply(const std::vector<Post3dWindowNodeVectorArrowDataItem::Setting>& ranges);

	std::vector<Post3dWindowNodeVectorArrowDataItem::Setting> m_newSettings;
	std::vector<Post3dWindowNodeVectorArrowDataItem::Setting> m_oldSettings;

	Post3dWindowNodeVectorArrowGroupDataItem* m_item;
};

#endif // POST3DWINDOWNODEVECTORARROWGROUPDATAITEM_SETFACESETTINGSCOMMAND_H
