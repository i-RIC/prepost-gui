#ifndef POST2DWINDOWGRIDATTRIBUTENODEGROUPDATAITEM_IMPL_H
#define POST2DWINDOWGRIDATTRIBUTENODEGROUPDATAITEM_IMPL_H

#include "../post2dwindowgridattributenodegroupdataitem.h"

#include <guicore/gridatt/node/gridattributenodesetting.h>

class Post2dWindowGridAttributeNodeGroupDataItem::Impl
{
public:
	Impl(Post2dWindowGridAttributeNodeGroupDataItem* item);
	~Impl();

	GridAttributeNodeSetting m_setting;

	std::string m_target;
	vtkActor* m_actor;

	QAction* m_showAttributeBrowserAction;
	bool m_attributeBrowserFixed;

	std::unordered_map<std::string, Post2dWindowGridAttributeNodeDataItem*> m_nameMap;

	OpacityContainerWidget* m_opacityWidget;
	QWidgetContainer* m_colorMapWidgetContainer;
};

#endif // POST2DWINDOWGRIDATTRIBUTENODEGROUPDATAITEM_IMPL_H
