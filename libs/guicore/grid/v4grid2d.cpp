#include "v4grid2d.h"
#include "private/v4grid2d_impl.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>

v4Grid2d::Impl::Impl() :
	m_vtkFilteredData {nullptr},
	m_vtkFilteredIndexData {nullptr},
	m_isMasked {false}
{}

v4Grid2d::Impl::~Impl()
{
	if (m_vtkFilteredData != nullptr) {
		m_vtkFilteredData->Delete();
	}
	if (m_vtkFilteredIndexData != nullptr) {
		m_vtkFilteredIndexData->Delete();
	}
}

v4Grid2d::v4Grid2d(vtkPointSetExtended* data) :
	v4Grid {data},
	impl {new Impl {}}
{}

v4Grid2d::~v4Grid2d()
{}

vtkPointSet* v4Grid2d::vtkFilteredData() const
{
	return impl->m_vtkFilteredData;
}

vtkPointSet* v4Grid2d::vtkFilteredIndexData() const
{
	return impl->m_vtkFilteredIndexData;
}

QPointF v4Grid2d::point2d(vtkIdType index) const
{
	double p[3];
	vtkData()->data()->GetPoints()->GetPoint(index, p);
	return QPointF(p[0], p[1]);
}

void v4Grid2d::setPoint2d(vtkIdType index, const QPointF& v)
{
	vtkData()->data()->GetPoints()->SetPoint(index, v.x(), v.y(), 0);
}

bool v4Grid2d::isMasked() const
{
	return impl->m_isMasked;
}

void v4Grid2d::setMasked(bool masked)
{
	impl->m_isMasked = masked;
}

void v4Grid2d::setFilteredData(vtkPointSet* data)
{
	if (impl->m_vtkFilteredData != nullptr) {
		impl->m_vtkFilteredData->Delete();
	}

	impl->m_vtkFilteredData = data;
	data->Register(nullptr);
}

void v4Grid2d::setFilteredIndexData(vtkPointSet* data)
{
	if (impl->m_vtkFilteredIndexData != nullptr) {
		impl->m_vtkFilteredIndexData->Delete();
	}

	impl->m_vtkFilteredIndexData = data;
	data->Register(nullptr);
}
