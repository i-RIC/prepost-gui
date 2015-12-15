#ifndef POST3DWINDOWPARTICLESSCALARDATAITEM_H
#define POST3DWINDOWPARTICLESSCALARDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>

class Post3dWindowParticlesScalarDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT
public:
	Post3dWindowParticlesScalarDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);

private:
	void doLoadFromProjectMainFile(const QDomNode& node) override;
	void doSaveToProjectMainFile(QXmlStreamWriter& writer) override;
};

#endif // POST3DWINDOWPARTICLESSCALARDATAITEM_H
