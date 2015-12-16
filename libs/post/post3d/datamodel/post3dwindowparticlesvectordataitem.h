#ifndef POST3DWINDOWPARTICLESVECTORDATAITEM_H
#define POST3DWINDOWPARTICLESVECTORDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>
#include "../post3dwindowdataitem.h"

class SolverDefinitionGridRelatedCondition;
class QAction;

class Post3dWindowParticlesVectorDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowParticlesVectorDataItem(const std::string& name, const QString& caption, Post3dWindowDataItem* parent);

private:
	void doLoadFromProjectMainFile(const QDomNode&) override;
	void doSaveToProjectMainFile(QXmlStreamWriter&) override;
};

#endif // POST3DWINDOWPARTICLESVECTORDATAITEM_H
