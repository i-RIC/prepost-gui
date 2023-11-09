#ifndef POST2DWINDOWPOLYDATAGROUPDATAITEM_IMPL_H
#define POST2DWINDOWPOLYDATAGROUPDATAITEM_IMPL_H

#include "../post2dwindowpolydatagroupdataitem.h"

class Post2dWindowPolyDataGroupDataItem::Impl
{
public:
	Impl(Post2dWindowPolyDataGroupDataItem* item);
	~Impl();

	vtkActor* m_actor;
	vtkActor2D* m_legendActor;

	QAction* m_showAttributeBrowserAction;
	AttributeBrowserController* m_attributeBrowserController;

	PolyDataSetting m_setting;
	std::unordered_map<std::string, ColorMapSettingContainerI*> m_colorMapSettings;
	PolyDataSettingToolBarWidget* m_polyDataToolBarWidget;
};

#endif // POST2DWINDOWPOLYDATAGROUPDATAITEM_IMPL_H
