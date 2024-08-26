#ifndef PREPROCESSORGRIDATTRIBUTENODEGROUPDATAITEM_IMPL_H
#define PREPROCESSORGRIDATTRIBUTENODEGROUPDATAITEM_IMPL_H

#include "../preprocessorgridattributenodegroupdataitem.h"

#include <guicore/gridatt/node/gridattributenodesetting.h>

class vtkActor;
class vtkActor2D;

class QWidget;

class PreProcessorGridAttributeNodeGroupDataItem::Impl
{
public:
	Impl(QWidget* mainWindow, PreProcessorGridAttributeNodeGroupDataItem* item);
	~Impl();

	GridAttributeNodeSetting m_setting;

	std::string m_target;
	vtkActor* m_actor;
	vtkActor2D* m_stringActor;

	QAction* m_showAttributeBrowserAction;
	bool m_attributeBrowserFixed;

	std::unordered_map<std::string, PreProcessorGridAttributeNodeDataItem*> m_nameMap;

	OpacityContainerWidget* m_opacityWidget;
	QWidgetContainer* m_colorMapWidgetContainer;
};

#endif // PREPROCESSORGRIDATTRIBUTENODEGROUPDATAITEM_IMPL_H
