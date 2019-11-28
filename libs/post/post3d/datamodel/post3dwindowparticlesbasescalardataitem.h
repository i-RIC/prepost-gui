#ifndef POST3DWINDOWPARTICLESBASESCALARDATAITEM_H
#define POST3DWINDOWPARTICLESBASESCALARDATAITEM_H

#include <guicore/named/namedgraphicwindowdataitem.h>

class Post3dWindowParticlesBaseScalarDataItem : public NamedGraphicWindowDataItem
{
	Q_OBJECT
public:
	Post3dWindowParticlesBaseScalarDataItem(const std::string& name, const QString& caption, GraphicsWindowDataItem* parent);
};

#endif // POST3DWINDOWPARTICLESBASESCALARDATAITEM_H
