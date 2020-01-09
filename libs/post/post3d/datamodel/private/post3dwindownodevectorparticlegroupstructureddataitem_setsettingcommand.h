#ifndef POST3DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_SETSETTINGCOMMAND_H
#define POST3DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_SETSETTINGCOMMAND_H

#include "../post3dwindownodevectorparticlegroupstructureddataitem.h"

#include <QUndoCommand>

class Post3dWindowNodeVectorParticleGroupStructuredDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const Post3dWindowNodeVectorParticleGroupDataItem::Setting& setting, const QList<Post3dWindowStructuredParticleSetSetting>& settings, Post3dWindowNodeVectorParticleGroupStructuredDataItem* item);

	void redo() override;
	void undo() override;

private:
	Post3dWindowNodeVectorParticleGroupDataItem::Setting m_newSetting;
	QList<Post3dWindowStructuredParticleSetSetting> m_newSettings;

	Post3dWindowNodeVectorParticleGroupDataItem::Setting m_oldSetting;
	QList<Post3dWindowStructuredParticleSetSetting> m_oldSettings;
	bool m_oldEnabled;

	Post3dWindowNodeVectorParticleGroupStructuredDataItem* m_item;
};

#endif // POST3DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_SETSETTINGCOMMAND_H
