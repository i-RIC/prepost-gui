#include "geodatapointmap_tinmanager_breakline.h"

GeoDataPointmap::TINManager::BreakLine::BreakLine(vtkPolyData* points) :
	m_points {points}
{
	auto linesProp = m_controller.linesActor()->GetProperty();
	linesProp->SetLighting(false);
	linesProp->SetLineWidth(3);
	linesProp->SetColor(0, 0, 0);

	auto pointsProp = m_controller.pointsActor()->GetProperty();
	pointsProp->SetLighting(false);
	pointsProp->SetPointSize(9);
	pointsProp->SetColor(0, 0, 0);

	m_controller.pointsActor()->VisibilityOff();
}

const std::vector<vtkIdType>& GeoDataPointmap::TINManager::BreakLine::vertexIndices() const
{
	return m_vertexIndices;
}

void GeoDataPointmap::TINManager::BreakLine::setVertexIndices(const std::vector<vtkIdType>& indices)
{
	m_vertexIndices = indices;
	updateShapeData();
}

void GeoDataPointmap::TINManager::BreakLine::cleanVertexIndices()
{
	auto indices = vertexIndices();
	std::vector<vtkIdType> newIndices;

	vtkIdType previous;

	if (indices.size() > 0) {
		previous = indices.at(0);
		newIndices.push_back(previous);
	}

	for (int i = 1; i < indices.size(); ++i) {
		auto v = indices.at(i);
		if (v != previous) {
			newIndices.push_back(v);
		}
		previous = v;
	}
	setVertexIndices(newIndices);
}

bool GeoDataPointmap::TINManager::BreakLine::intersects(const BreakLine& line) const
{
	auto l1 = m_controller.polyLine();
	auto l2 = line.controller().polyLine();

	for (int i = 0; i < static_cast<int>(l1.size()) - 1; ++i){
		QLineF line1(l1.at(i), l1.at(i + 1));
		for (int j = 0; j < static_cast<int>(l2.size()) - 1; ++j) {
			QLineF line2(l2.at(j), l2.at(j + 1));

			QPointF intersectionPoint;
			auto type = line1.intersect(line2, &intersectionPoint);

			if (type != QLineF::BoundedIntersection) {continue;}
			if (intersectionPoint == l2.at(j)) {continue;}
			if (intersectionPoint == l2.at(j + 1)) {continue;}

			return true;
		}
	}
	return false;
}

const PolyLineController& GeoDataPointmap::TINManager::BreakLine::controller() const
{
	return m_controller;
}

void GeoDataPointmap::TINManager::BreakLine::addActorsToRenderer(vtkRenderer* renderer)
{
	renderer->AddActor(m_controller.linesActor());
	renderer->AddActor(m_controller.pointsActor());
}

void GeoDataPointmap::TINManager::BreakLine::removeActorsFromRenderer(vtkRenderer* renderer)
{
	renderer->RemoveActor(m_controller.linesActor());
	renderer->RemoveActor(m_controller.pointsActor());
}

void GeoDataPointmap::TINManager::BreakLine::updateShapeData()
{
	m_controller.pointsPolyData()->Initialize();
	m_controller.polyData()->Initialize();

	auto points = m_points->GetPoints();
	m_controller.pointsPolyData()->SetPoints(points);
	m_controller.polyData()->SetPoints(points);

	// build edges
	auto edges = vtkSmartPointer<vtkCellArray>::New();
	vtkIdType edgeCount = m_vertexIndices.size() - 1;
	if (edgeCount > 0) {
		edges->Allocate(edgeCount);
		vtkIdType indices[2];
		for (vtkIdType i = 0; i < edgeCount; ++i) {
			indices[0] = m_vertexIndices.at(i);
			indices[1] = m_vertexIndices.at(i + 1);
			edges->InsertNextCell(2, indices);
		}
		m_controller.polyData()->SetLines(edges);
	}

	// build points
	auto verts = vtkSmartPointer<vtkCellArray>::New();
	for (vtkIdType index : m_vertexIndices) {
		verts->InsertNextCell(1, &index);
	}
	m_controller.pointsPolyData()->SetVerts(verts);
}
