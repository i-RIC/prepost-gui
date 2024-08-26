#ifndef POST2DWINDOWGRIDATTRIBUTEABSTRACTCELLGROUPDATAITEM_IMPL_H
#define POST2DWINDOWGRIDATTRIBUTEABSTRACTCELLGROUPDATAITEM_IMPL_H

#include "../post2dwindowgridattributeabstractcellgroupdataitem.h"

#include <guicore/gridatt/cell/gridattributecellsetting.h>

class Post2dWindowGridAttributeAbstractCellGroupDataItem::Impl
{
public:
	Impl(Post2dWindowGridAttributeAbstractCellGroupDataItem* item);
	~Impl();

	GridAttributeCellSetting m_setting;

	std::string m_target;
	vtkActor* m_actor;
	vtkActor2D* m_stringActor;

	std::unordered_map<std::string, Post2dWindowGridAttributeAbstractCellDataItem*> m_nameMap;

	QAction* m_showAttributeBrowserAction;
	bool m_attributeBrowserFixed;

	OpacityContainerWidget* m_opacityWidget;
	QWidgetContainer* m_colorMapWidgetContainer;
};

#endif // POST2DWINDOWGRIDATTRIBUTEABSTRACTCELLGROUPDATAITEM_IMPL_H
