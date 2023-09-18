#ifndef POST2DWINDOWPARTICLESBASETOPDATAITEM_IMPL_H
#define POST2DWINDOWPARTICLESBASETOPDATAITEM_IMPL_H

#include "../post2dwindowparticlesbasetopdataitem.h"

class Post2dWindowParticlesBaseTopDataItem::Impl
{
public:
	Impl(Post2dWindowParticlesBaseTopDataItem* item);
	~Impl();

	AttributeBrowserController* m_attributeBrowserController;

	Post2dWindowParticlesBaseScalarGroupDataItem* m_scalarGroupDataItem;
	Post2dWindowParticlesBaseVectorGroupTopDataItem* m_vectorGroupDataItem;

	QAction* m_showAttributeBrowserAction;
};

#endif // POST2DWINDOWPARTICLESBASETOPDATAITEM_IMPL_H
