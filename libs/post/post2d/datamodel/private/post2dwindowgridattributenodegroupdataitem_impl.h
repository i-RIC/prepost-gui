#ifndef POST2DWINDOWGRIDATTRIBUTENODEGROUPDATAITEM_IMPL_H
#define POST2DWINDOWGRIDATTRIBUTENODEGROUPDATAITEM_IMPL_H

#include "../post2dwindowgridattributenodegroupdataitem.h"

class Post2dWindowGridAttributeNodeGroupDataItem::Impl
{
public:
	Impl(Post2dWindowGridAttributeNodeGroupDataItem* item);
	~Impl();

	std::string m_target;

	vtkActor* m_actor;

	QAction* m_showAttributeBrowserAction;
	OpacityContainer m_opacity;
	bool m_attributeBrowserFixed;
	std::unordered_map<std::string, Post2dWindowGridAttributeNodeDataItem*> m_nameMap;

	OpacityContainerWidget* m_opacityWidget;
	QWidgetContainer* m_colorMapWidgetContainer;
};

#endif // POST2DWINDOWGRIDATTRIBUTENODEGROUPDATAITEM_IMPL_H
