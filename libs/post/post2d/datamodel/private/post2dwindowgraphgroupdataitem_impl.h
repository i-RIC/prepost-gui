#ifndef POST2DWINDOWGRAPHGROUPDATAITEM_IMPL_H
#define POST2DWINDOWGRAPHGROUPDATAITEM_IMPL_H

#include "../post2dwindowgraphgroupdataitem.h"
#include "../post2dwindowgraphsetting.h"

#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

class Post2dWindowGraphGroupDataItem::Impl
{
public:
	Impl();

	Post2dWindowGraphSetting m_setting;

	vtkSmartPointer<vtkActor> m_baseLinesActor;
	vtkSmartPointer<vtkActor> m_graphLinesActor;

	vtkSmartPointer<vtkPolyData> m_baseLinesPolyData;
	vtkSmartPointer<vtkPolyData> m_graphLinesPolyData;
};

#endif // POST2DWINDOWGRAPHGROUPDATAITEM_IMPL_H
