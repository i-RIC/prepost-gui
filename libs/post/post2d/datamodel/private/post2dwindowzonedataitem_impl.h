#ifndef POST2DWINDOWZONEDATAITEM_IMPL_H
#define POST2DWINDOWZONEDATAITEM_IMPL_H

#include "../post2dwindowzonedataitem.h"

class vtkActor;
class vtkPolyData;
class vtkPolyDataMapper;

class Post2dWindowZoneDataItem::Impl
{
public:
	Impl(const std::string& zoneName);
	~Impl();

	vtkPolyData* m_regionPolyData;
	vtkPolyDataMapper* m_regionMapper;
	vtkActor* m_regionActor;

	Post2dWindowInputGridDataItem* m_inputGridDataItem;
	Post2dWindowCalculationResultDataItem* m_resultDataItem;

	std::string m_zoneName;
};

#endif // POST2DWINDOWZONEDATAITEM_IMPL_H
