#ifndef POST3DWINDOWNODEVECTORPARTICLEDATAITEM_H
#define POST3DWINDOWNODEVECTORPARTICLEDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>

class Post3dWindowNodeVectorParticleDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT
public:
	Post3dWindowNodeVectorParticleDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);

private:
	void doLoadFromProjectMainFile(const QDomNode&) override;
	void doSaveToProjectMainFile(QXmlStreamWriter&) override;
};

#endif // POST3DWINDOWNODEVECTORPARTICLEDATAITEM_H
