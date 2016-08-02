#ifndef POST2DWINDOWGRAPHGROUPDATAITEM_IMPL_H
#define POST2DWINDOWGRAPHGROUPDATAITEM_IMPL_H

#include "../post2dwindowgraphgroupdataitem.h"
#include "../post2dwindowgraphsetting.h"

#include <guibase/vtktool/vtkpolydatalinesactor.h>
#include <guibase/vtktool/vtklinesactor.h>

#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

class Post2dWindowGraphGroupDataItem::Impl
{
public:
	Impl();

	Post2dWindowGraphSetting m_setting;

	vtkPolyDataLinesActor m_baseLinesActor;
	vtkSmartPointer<vtkPolyData> m_baseLinesPolyData;

	vtkLinesActor m_graphLinesActor;
};

#endif // POST2DWINDOWGRAPHGROUPDATAITEM_IMPL_H
