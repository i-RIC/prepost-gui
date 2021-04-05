#ifndef POST3DWINDOWNODEVECTORARROWTOPDATAITEM_CREATECOMMAND_H
#define POST3DWINDOWNODEVECTORARROWTOPDATAITEM_CREATECOMMAND_H

#include "../post3dwindownodevectorarrowgroupdataitem.h"
#include "../post3dwindownodevectorarrowsettingdialog.h"
#include "../post3dwindownodevectorarrowtopdataitem.h"

#include <QUndoCommand>

class Post3dWindowNodeVectorArrowTopDataItem::CreateCommand : public QUndoCommand
{
public:
	CreateCommand(const ArrowSettingContainer& arrowSetting, const std::vector<Post3dWindowNodeVectorArrowSettingDialog::FaceSetting>& faceSettings, Post3dWindowNodeVectorArrowTopDataItem* item);
	~CreateCommand();

	void redo() override;
	void undo() override;

private:
	ArrowSettingContainer m_arrowSetting;
	std::vector<Post3dWindowNodeVectorArrowSettingDialog::FaceSetting> m_faceSettings;

	Post3dWindowNodeVectorArrowGroupDataItem* m_newItem;
	Post3dWindowNodeVectorArrowTopDataItem* m_item;
};

#endif // POST3DWINDOWNODEVECTORARROWTOPDATAITEM_CREATECOMMAND_H
