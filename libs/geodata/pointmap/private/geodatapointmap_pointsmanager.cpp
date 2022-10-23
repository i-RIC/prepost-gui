#include "geodatapointmap_pointsmanager.h"

#include <vtkLODActor.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>

GeoDataPointmap::PointsManager::PointsManager(GeoDataPointmap* pointmap) :
	m_points {vtkPolyData::New()},
	m_pointsActor {vtkLODActor::New()},
	m_delaunayedTrianglesActor {vtkLODActor::New()},
	m_maskPoints10k {vtkMaskPoints::New()},
	m_maskPoints40k {vtkMaskPoints::New()},
	m_parent {pointmap}
{
	setupSettings();
}

GeoDataPointmap::PointsManager::~PointsManager()
{
	m_points->Delete();
	m_pointsActor->Delete();
	m_delaunayedTrianglesActor->Delete();
	m_maskPoints10k->Delete();
	m_maskPoints40k->Delete();
}

vtkPolyData* GeoDataPointmap::PointsManager::points() const
{
	return m_points;
}

vtkLODActor* GeoDataPointmap::PointsManager::pointsActor() const
{
	return m_pointsActor;
}

vtkLODActor* GeoDataPointmap::PointsManager::delaunayedTrianglesActor() const
{
	return m_delaunayedTrianglesActor;
}

vtkMaskPoints* GeoDataPointmap::PointsManager::maskPoints10k() const
{
	return m_maskPoints10k;
}

vtkMaskPoints* GeoDataPointmap::PointsManager::maskPoints40k() const
{
	return m_maskPoints40k;
}

void GeoDataPointmap::PointsManager::setupSettings()
{
	// m_points
	auto points = vtkPoints::New();
	points->SetDataTypeToDouble();
	m_points->SetPoints(points);
	points->Delete();

	/*
	// m_pointsActor
	m_pointsActor->SetMapper(m_pointsMapper);
	m_pointsActor->AddLODMapper(m_maskPoints40kMapper);
	m_pointsActor->AddLODMapper(m_maskPoints10kMapper);
	*/

	auto p = m_pointsActor->GetProperty();
	p->SetPointSize(m_parent->m_pointSize);
	p->SetRepresentationToPoints();
	p->SetLighting(false);

	/*
	// m_pointsMapper
	m_pointsMapper->SetUseLookupTableScalarRange(true);
	m_pointsMapper->SetScalarVisibility(true);
	m_pointsMapper->SetScalarModeToUsePointData();
	*/

	// m_delaunayedTrianglesActor
	/*
	m_delaunayedTrianglesActor->SetMapper(m_delaunayedTrianglesMapper);
	m_delaunayedTrianglesActor->AddLODMapper(m_pointsMapper);
	m_delaunayedTrianglesActor->AddLODMapper(m_maskPoints40kMapper);
	m_delaunayedTrianglesActor->AddLODMapper(m_maskPoints10kMapper);
	*/

	p = m_delaunayedTrianglesActor->GetProperty();
	p->SetRepresentationToPoints();
	p->SetLighting(false);

	/*
	// m_delaunayedTrianglesMapper
	m_delaunayedTrianglesMapper->SetUseLookupTableScalarRange(true);
	m_delaunayedTrianglesMapper->SetScalarVisibility(true);
	*/

	// m_maskPoints10k
	m_maskPoints10k->RandomModeOn();
	m_maskPoints10k->SetMaximumNumberOfPoints(10000);
	m_maskPoints10k->GenerateVerticesOn();
	m_maskPoints10k->SetInputData(m_points);

	/*
	// m_maskPoints10kMapper
	m_maskPoints10kMapper->SetInputConnection(m_maskPoints10k->GetOutputPort());
	m_maskPoints10kMapper->SetUseLookupTableScalarRange(true);
	m_maskPoints10kMapper->SetScalarVisibility(true);
	m_maskPoints10kMapper->SetScalarModeToUsePointData();
	*/

	// m_maskPoints40k
	m_maskPoints40k->RandomModeOn();
	m_maskPoints40k->SetMaximumNumberOfPoints(40000);
	m_maskPoints40k->GenerateVerticesOn();
	m_maskPoints40k->SetInputData(m_points);

	/*
	// m_maskPoints40kMapper
	m_maskPoints40kMapper->SetInputConnection(m_maskPoints40k->GetOutputPort());
	m_maskPoints40kMapper->SetUseLookupTableScalarRange(true);
	m_maskPoints40kMapper->SetScalarVisibility(true);
	m_maskPoints40kMapper->SetScalarModeToUsePointData();
	*/
}
