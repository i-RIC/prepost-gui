#include "v4gridutil.h"
#include "v4structured3dgrid.h"
#include "private/v4structured3dgrid_impl.h"

#include <misc/stringtool.h>

#include <vtkPolyData.h>

v4Structured3dGrid::Impl::Impl(v4Structured3dGrid* parent) :
	m_vtkIndexData {vtkPolyData::New()},
	m_vtkIFaceData {},
	m_vtkJFaceData {},
	m_vtkKFaceData {},
	m_dimensionI {0},
	m_dimensionJ {0},
	m_dimensionK {0},
	m_parent {parent}
{
	m_vtkIndexData->SetPoints(m_parent->vtkData()->data()->GetPoints());

	m_vtkIFaceData.data()->SetPoints(m_parent->vtkData()->data()->GetPoints());
	m_vtkJFaceData.data()->SetPoints(m_parent->vtkData()->data()->GetPoints());
	m_vtkKFaceData.data()->SetPoints(m_parent->vtkData()->data()->GetPoints());
}

v4Structured3dGrid::Impl::~Impl()
{
	m_vtkIndexData->Delete();
}

void v4Structured3dGrid::Impl::setupVtkObjects()
{
	auto grid = m_parent->vtkConcreteData()->concreteData();
	grid->SetDimensions(m_dimensionI, m_dimensionJ, m_dimensionK);

	auto pointCount = m_dimensionI * m_dimensionJ * m_dimensionK;
	auto points = grid->GetPoints();
	points->Initialize();
	points->Allocate(pointCount);

	for (int i = 0; i < pointCount; ++i) {
		points->InsertNextPoint(0, 0, 0);
	}

	vtkIdType pointId = 0;
	QString label("(%1,%2,%3)");
	auto ca = vtkSmartPointer<vtkCellArray>::New();
	auto sa = vtkSmartPointer<vtkStringArray>::New();
	sa->SetName(v4GridUtil::LABEL_NAME);

	for (int i = 0; i < m_dimensionI; ++i) {
		pointId = m_parent->pointIndex(i, 0, 0);
		ca->InsertNextCell(1, &pointId);
		sa->InsertNextValue(iRIC::toStr(label.arg(i + 1).arg(1).arg(1)));
	}
	for (int j = 0; j < m_dimensionJ; ++j) {
		pointId = m_parent->pointIndex(0, j, 0);
		ca->InsertNextCell(1, &pointId);
		sa->InsertNextValue(iRIC::toStr(label.arg(1).arg(j + 1).arg(1)));
	}
	for (int k = 0; k < m_dimensionK; ++k) {
		pointId = m_parent->pointIndex(0, 0, k);
		ca->InsertNextCell(1, &pointId);
		sa->InsertNextValue(iRIC::toStr(label.arg(1).arg(1).arg(k + 1)));
	}
	m_vtkIndexData->SetVerts(ca);
	m_vtkIndexData->GetCellData()->AddArray(sa);

	vtkIdType indices[4];

	auto iFacePolys = vtkSmartPointer<vtkCellArray>::New();
	iFacePolys->Allocate((m_dimensionI) * (m_dimensionJ - 1) * (m_dimensionK - 1));
	for (int k = 0; k < m_dimensionK - 1; ++k) {
		for (int j = 0; j < m_dimensionJ - 1; ++j) {
			for (int i = 0; i < m_dimensionI; ++i) {
				indices[0] = m_parent->pointIndex(i, j, k);
				indices[1] = m_parent->pointIndex(i, j + 1, k);
				indices[2] = m_parent->pointIndex(i, j + 1, k + 1);
				indices[3] = m_parent->pointIndex(i, j, k + 1);
				iFacePolys->InsertNextCell(4, indices);
			}
		}
	}
	m_vtkIFaceData.concreteData()->SetPolys(iFacePolys);

	auto jFacePolys = vtkSmartPointer<vtkCellArray>::New();
	jFacePolys->Allocate((m_dimensionI - 1) * (m_dimensionJ) * (m_dimensionK - 1));
	for (int k = 0; k < m_dimensionK - 1; ++k) {
		for (int j = 0; j < m_dimensionJ; ++j) {
			for (int i = 0; i < m_dimensionI - 1; ++i) {
				indices[0] = m_parent->pointIndex(i, j, k);
				indices[1] = m_parent->pointIndex(i + 1, j, k);
				indices[2] = m_parent->pointIndex(i + 1, j, k + 1);
				indices[3] = m_parent->pointIndex(i, j, k + 1);
				jFacePolys->InsertNextCell(4, indices);
			}
		}
	}
	m_vtkJFaceData.concreteData()->SetPolys(jFacePolys);

	auto kFacePolys = vtkSmartPointer<vtkCellArray>::New();
	kFacePolys->Allocate((m_dimensionI - 1) * (m_dimensionJ - 1) * (m_dimensionK));
	for (int k = 0; k < m_dimensionK; ++k) {
		for (int j = 0; j < m_dimensionJ - 1; ++j) {
			for (int i = 0; i < m_dimensionI - 1; ++i) {
				indices[0] = m_parent->pointIndex(i, j, k);
				indices[1] = m_parent->pointIndex(i + 1, j, k);
				indices[2] = m_parent->pointIndex(i + 1, j + 1, k);
				indices[3] = m_parent->pointIndex(i, j + 1, k);
				kFacePolys->InsertNextCell(4, indices);
			}
		}
	}
	m_vtkKFaceData.concreteData()->SetPolys(kFacePolys);
}

v4Structured3dGrid::v4Structured3dGrid() :
	v4Grid3dT<vtkStructuredGridExtended3d> {},
	impl {new Impl {this}}
{}

v4Structured3dGrid::~v4Structured3dGrid()
{}

vtkPointSet* v4Structured3dGrid::vtkIndexData() const
{
	return impl->m_vtkIndexData;
}

void v4Structured3dGrid::setDimensions(vtkIdType dimI, vtkIdType dimJ, vtkIdType dimK)
{
	impl->m_dimensionI = dimI;
	impl->m_dimensionJ = dimJ;
	impl->m_dimensionK = dimK;

	impl->setupVtkObjects();
}

void v4Structured3dGrid::getDimensions(vtkIdType* dimI, vtkIdType* dimJ, vtkIdType* dimK) const
{
	*dimI = impl->m_dimensionI;
	*dimJ = impl->m_dimensionJ;
	*dimK = impl->m_dimensionK;
}

vtkIdType v4Structured3dGrid::dimensionI() const
{
	return impl->m_dimensionI;
}

vtkIdType v4Structured3dGrid::dimensionJ() const
{
	return impl->m_dimensionJ;
}

vtkIdType v4Structured3dGrid::dimensionK() const
{
	return impl->m_dimensionK;
}

vtkIdType v4Structured3dGrid::pointIndex(vtkIdType i, vtkIdType j, vtkIdType k) const
{
	return impl->m_dimensionI * impl->m_dimensionJ * k + impl->m_dimensionI * j + i;
}

void v4Structured3dGrid::getPointIJKIndex(vtkIdType index, vtkIdType* i, vtkIdType* j, vtkIdType* k) const
{
	*i = index % (impl->m_dimensionI);
	*j = (index - *i) % (impl->m_dimensionI * impl->m_dimensionJ);
	*k = index / (impl->m_dimensionI * impl->m_dimensionJ);
}

vtkIdType v4Structured3dGrid::cellIndex(vtkIdType i, vtkIdType j, vtkIdType k) const
{
	return (impl->m_dimensionI - 1) * (impl->m_dimensionJ - 1) * k + (impl->m_dimensionI - 1) * j + i;
}

void v4Structured3dGrid::getCellIJKIndex(vtkIdType index, vtkIdType* i, vtkIdType* j, vtkIdType* k) const
{
	*i = index % (impl->m_dimensionI - 1);
	*j = (index - *i) % ((impl->m_dimensionI - 1) * (impl->m_dimensionJ - 1));
	*k = index / ((impl->m_dimensionI - 1) * (impl->m_dimensionJ - 1));
}

vtkIdType v4Structured3dGrid::iFaceIndex(vtkIdType i, vtkIdType j, vtkIdType k) const
{
	return (impl->m_dimensionI) * (impl->m_dimensionJ - 1) * k + (impl->m_dimensionI) * j + i;
}

void v4Structured3dGrid::getIFaceIJKIndex(vtkIdType index, vtkIdType* i, vtkIdType* j, vtkIdType* k) const
{
	*i = index % (impl->m_dimensionI);
	*j = (index - *i) % ((impl->m_dimensionI) * (impl->m_dimensionJ - 1));
	*k = index / ((impl->m_dimensionI) * (impl->m_dimensionJ - 1));
}

vtkIdType v4Structured3dGrid::jFaceIndex(vtkIdType i, vtkIdType j, vtkIdType k) const
{
	return (impl->m_dimensionI - 1) * (impl->m_dimensionJ) * k + (impl->m_dimensionI - 1) * j + i;
}

void v4Structured3dGrid::getJFaceIJKIndex(vtkIdType index, vtkIdType* i, vtkIdType* j, vtkIdType* k) const
{
	*i = index % (impl->m_dimensionI - 1);
	*j = (index - *i) % ((impl->m_dimensionI - 1) * (impl->m_dimensionJ));
	*k = index / ((impl->m_dimensionI - 1) * (impl->m_dimensionJ));
}

vtkIdType v4Structured3dGrid::kFaceIndex(vtkIdType i, vtkIdType j, vtkIdType k) const
{
	return (impl->m_dimensionI - 1) * (impl->m_dimensionJ - 1) * k + (impl->m_dimensionI - 1) * j + i;
}

void v4Structured3dGrid::getKFaceIJKIndex(vtkIdType index, vtkIdType* i, vtkIdType* j, vtkIdType* k) const
{
	*i = index % (impl->m_dimensionI - 1);
	*j = (index - *i) % ((impl->m_dimensionI - 1) * (impl->m_dimensionJ - 1));
	*k = index / ((impl->m_dimensionI - 1) * (impl->m_dimensionJ - 1));
}

vtkPolyDataExtended3d* v4Structured3dGrid::vtkIFaceData() const
{
	return &impl->m_vtkIFaceData;
}

vtkPolyDataExtended3d* v4Structured3dGrid::vtkJFaceData() const
{
	return &impl->m_vtkJFaceData;
}

vtkPolyDataExtended3d* v4Structured3dGrid::vtkKFaceData() const
{
	return &impl->m_vtkKFaceData;
}
