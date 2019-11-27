#ifndef POST2DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_SETSETTINGCOMMAND_H
#define POST2DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_SETSETTINGCOMMAND_H

#include "../post2dwindownodevectorparticlegroupstructureddataitem.h"

#include <QUndoCommand>

class Post2dWindowNodeVectorParticleGroupStructuredDataItem::SetSettingCommand  : public QUndoCommand
{
public:
	SetSettingCommand(const Post2dWindowNodeVectorParticleGroupDataItem::Setting& s, const QList<Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting>& settings, Post2dWindowNodeVectorParticleGroupStructuredDataItem* item);

	void redo() override;
	void undo() override;

private:
	Post2dWindowNodeVectorParticleGroupDataItem::Setting m_newSetting;
	QList<Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting> m_newStSettings;

	bool m_oldEnabled;
	Post2dWindowNodeVectorParticleGroupDataItem::Setting m_oldSetting;
	QList<Post2dWindowNodeVectorParticleGroupStructuredDataItem::Setting> m_oldStSettings;

	Post2dWindowNodeVectorParticleGroupStructuredDataItem* m_item;
};

#endif // POST2DWINDOWNODEVECTORPARTICLEGROUPSTRUCTUREDDATAITEM_SETSETTINGCOMMAND_H
