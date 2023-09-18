#ifndef POST2DWINDOWGRIDATTRIBUTEABSTRACTCELLGROUPDATAITEM_IMPL_H
#define POST2DWINDOWGRIDATTRIBUTEABSTRACTCELLGROUPDATAITEM_IMPL_H

#include "../post2dwindowgridattributeabstractcellgroupdataitem.h"

class Post2dWindowGridAttributeAbstractCellGroupDataItem::Impl
{
public:
	Impl(Post2dWindowGridAttributeAbstractCellGroupDataItem* item);
	~Impl();

	std::string m_target;
	vtkActor* m_actor;
	std::unordered_map<std::string, Post2dWindowGridAttributeAbstractCellDataItem*> m_nameMap;

	IntContainer m_lineWidth;
	OpacityContainer m_opacity;
	QAction* m_showAttributeBrowserAction;
	bool m_attributeBrowserFixed;

	OpacityContainerWidget* m_opacityWidget;
	QWidgetContainer* m_colorMapWidgetContainer;
};

#endif // POST2DWINDOWGRIDATTRIBUTEABSTRACTCELLGROUPDATAITEM_IMPL_H
