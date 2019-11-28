#ifndef POST3DWINDOWPARTICLESBASEVECTORDATAITEM_H
#define POST3DWINDOWPARTICLESBASEVECTORDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>
#include "../post3dwindowdataitem.h"

class SolverDefinitionGridRelatedCondition;
class QAction;

class Post3dWindowParticlesBaseVectorDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT

public:
	Post3dWindowParticlesBaseVectorDataItem(const std::string& name, const QString& caption, Post3dWindowDataItem* parent);

private:
	void doLoadFromProjectMainFile(const QDomNode&) override;
	void doSaveToProjectMainFile(QXmlStreamWriter&) override;
};

#endif // POST3DWINDOWPARTICLESBASEVECTORDATAITEM_H
