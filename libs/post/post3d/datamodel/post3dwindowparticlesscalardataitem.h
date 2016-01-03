#ifndef POST3DWINDOWPARTICLESSCALARDATAITEM_H
#define POST3DWINDOWPARTICLESSCALARDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>

class Post3dWindowParticlesScalarDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT
public:
	Post3dWindowParticlesScalarDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);
};

#endif // POST3DWINDOWPARTICLESSCALARDATAITEM_H
