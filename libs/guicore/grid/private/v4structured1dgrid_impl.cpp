#include "v4structured1dgrid_impl.h"

v4Structured1dGrid::Impl::Impl(v4Structured1dGrid* parent) :
	m_dimension {0},
	m_parent {parent}
{}

v4Structured1dGrid::Impl::~Impl()
{}

void v4Structured1dGrid::Impl::setupVtkObjects()
{
	auto grid = m_parent->vtkConcreteData()->concreteData();
	grid->SetDimensions(m_dimension, 1, 1);

	auto pointCount = m_dimension;
	auto points = grid->GetPoints();
	points->Initialize();
	points->Allocate(pointCount);

	for (int i = 0; i < pointCount; ++i) {
		points->InsertNextPoint(0, 0, 0);
	}
}
