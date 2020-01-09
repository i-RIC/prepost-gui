#ifndef POST2DWINDOWNODEVECTORPARTICLEGROUPUNSTRUCTUREDDATAITEM_SETSETTINGCOMMAND_H
#define POST2DWINDOWNODEVECTORPARTICLEGROUPUNSTRUCTUREDDATAITEM_SETSETTINGCOMMAND_H

#include "../post2dwindownodevectorparticlegroupunstructureddataitem.h"

#include <QUndoCommand>

class Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::SetSettingCommand : public QUndoCommand
{
public:
	SetSettingCommand(const Post2dWindowNodeVectorParticleGroupDataItem::Setting& s, const QList<Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting>& unsts, Post2dWindowNodeVectorParticleGroupUnstructuredDataItem* item);

	void redo() override;
	void undo() override;

private:
	Post2dWindowNodeVectorParticleGroupDataItem::Setting m_newSetting;
	QList<Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting> m_newUnstSettings;

	bool m_oldEnabled;
	Post2dWindowNodeVectorParticleGroupDataItem::Setting m_oldSetting;
	QList<Post2dWindowNodeVectorParticleGroupUnstructuredDataItem::Setting> m_oldUnstSettings;

	Post2dWindowNodeVectorParticleGroupUnstructuredDataItem* m_item;
};

#endif // POST2DWINDOWNODEVECTORPARTICLEGROUPUNSTRUCTUREDDATAITEM_SETSETTINGCOMMAND_H
