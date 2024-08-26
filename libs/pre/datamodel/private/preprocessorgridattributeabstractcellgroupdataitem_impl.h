#ifndef PREPROCESSORGRIDATTRIBUTEABSTRACTCELLGROUPDATAITEM_IMPL_H
#define PREPROCESSORGRIDATTRIBUTEABSTRACTCELLGROUPDATAITEM_IMPL_H

#include "../preprocessorgridattributeabstractcellgroupdataitem.h"

#include <guicore/gridatt/cell/gridattributecellsetting.h>

class vtkActor;
class vtkActor2D;

class PreProcessorGridAttributeAbstractCellGroupDataItem::Impl
{
public:
	Impl(QWidget* mainWindow, PreProcessorGridAttributeAbstractCellGroupDataItem* item);
	~Impl();

	GridAttributeCellSetting m_setting;

	std::string m_target;
	vtkActor* m_actor;
	vtkActor2D* m_stringActor;

	QAction* m_showAttributeBrowserAction;
	bool m_attributeBrowserFixed;

	std::unordered_map<std::string, PreProcessorGridAttributeAbstractCellDataItem*> m_nameMap;

	OpacityContainerWidget* m_opacityWidget;
	QWidgetContainer* m_colorMapWidgetContainer;
};

#endif // PREPROCESSORGRIDATTRIBUTEABSTRACTCELLGROUPDATAITEM_IMPL_H
