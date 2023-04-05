#include "geodatapointmap.h"

#include "geodatapointmaprealbuilder.h"
#include "private/geodatapointmaprealbuilder_impl.h"

#include <guicore/pre/geodata/geodatacreator.h>

#include <vtkDoubleArray.h>

GeoDataPointmapRealBuilder::Impl::Impl() :
	m_points {nullptr},
	m_values {nullptr}
{}

GeoDataPointmapRealBuilder::Impl::~Impl()
{
	if (m_points != nullptr) {
		m_points->Delete();
	}
	if (m_values != nullptr) {
		m_values->Delete();
	}
}

GeoDataPointmapRealBuilder::GeoDataPointmapRealBuilder() :
	impl {new Impl()}
{}

GeoDataPointmapRealBuilder::~GeoDataPointmapRealBuilder()
{
	delete impl;
}

void GeoDataPointmapRealBuilder::begin()
{
	impl->m_points = vtkPoints::New(VTK_DOUBLE);
	impl->m_values = vtkDoubleArray::New();
}

GeoData* GeoDataPointmapRealBuilder::end(ProjectDataItem* parent, GeoDataCreator* creator, SolverDefinitionGridAttribute* condition)
{
	auto geoData = creator->create(parent, condition);
	auto pointMap = dynamic_cast<GeoDataPointmap*> (geoData);
	pointMap->setupActors();

	pointMap->setPoints(impl->m_points, impl->m_values);
	pointMap->rebuildTinFromPoints(false);

	impl->m_points->Delete();
	impl->m_points = nullptr;

	impl->m_values->Delete();
	impl->m_values = nullptr;

	return geoData;
}

void GeoDataPointmapRealBuilder::addPoint(double x, double y, double z)
{
	impl->m_points->InsertNextPoint(x, y, 0);
	impl->m_values->InsertNextValue(z);
}
