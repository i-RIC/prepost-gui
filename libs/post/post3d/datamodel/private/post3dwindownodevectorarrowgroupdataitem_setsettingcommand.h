#ifndef POST3DWINDOWNODEVECTORARROWGROUPDATAITEM_SETSETTINGCOMMAND_H
#define POST3DWINDOWNODEVECTORARROWGROUPDATAITEM_SETSETTINGCOMMAND_H

#include "../post3dwindownodevectorarrowgroupdataitem.h"
#include "../post3dwindownodevectorarrowsettingdialog.h"

#include <QUndoCommand>

#include <vector>

class Post3dWindowNodeVectorArrowGroupDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const ArrowSettingContainer& arrowSetting, const std::vector<Post3dWindowNodeVectorArrowSettingDialog::FaceSetting>& faceSettings, Post3dWindowNodeVectorArrowGroupDataItem* item);
	~SetSettingCommand();

	void redo() override;
	void undo() override;

private:
	void applySetting(const ArrowSettingContainer& arrowSetting, const std::vector<Post3dWindowNodeVectorArrowSettingDialog::FaceSetting>& faceSettings);

	ArrowSettingContainer m_newArrowSetting;
	std::vector<Post3dWindowNodeVectorArrowSettingDialog::FaceSetting> m_newFaceSettings;

	ArrowSettingContainer m_oldArrowSetting;
	std::vector<Post3dWindowNodeVectorArrowSettingDialog::FaceSetting> m_oldFaceSettings;

	Post3dWindowNodeVectorArrowGroupDataItem* m_item;
};

#endif // POST3DWINDOWNODEVECTORARROWGROUPDATAITEM_SETSETTINGCOMMAND_H
