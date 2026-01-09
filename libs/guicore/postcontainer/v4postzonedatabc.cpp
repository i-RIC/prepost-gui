#include "v4postzonedatabc.h"

#include "../solverdef/solverdefinitionboundarycondition.h"

#include <h5cgnsbc.h>

v4PostZoneDataBC::v4PostZoneDataBC(SolverDefinitionBoundaryCondition* condition, const iRICLib::H5CgnsZone& zone, const iRICLib::H5CgnsBc& bc) :
	m_indices {},
	m_edges {},
	m_caption {},
	m_defaultOpacity {},
	m_defaultColor {},
	m_defaultShowName {},
	m_defaultPointSize {},
	m_condition {condition}
{
	std::vector<int> indices;
	bc.readIndices(&indices);

	setupIndicesAndEdges(zone, indices);

	int captionLen;
	bc.readStringLen("_caption", &captionLen);
	std::vector<char> caption(captionLen + 1, 0);
	bc.readString("_caption", caption.data());
	m_caption = caption.data();

	int opacity;
	bc.readIntegerValue("_opacity", &opacity);
	m_defaultOpacity = opacity;

	int colorStrLen;
	bc.readStringLen("_color", &colorStrLen);
	std::vector<char> colorStr(colorStrLen + 1, 0);
	bc.readString("_color", colorStr.data());
	m_defaultColor = QColor(colorStr.data());

	int showname;
	bc.readIntegerValue("_showname", &showname);
	m_defaultShowName = (showname == 1);

	bc.readIntegerValue("_pointsize", &m_defaultPointSize);
}

const std::unordered_set<vtkIdType>& v4PostZoneDataBC::indices() const
{
	return m_indices;
}

const QSet<Edge>& v4PostZoneDataBC::edges() const
{
	return m_edges;
}

const std::string& v4PostZoneDataBC::caption() const
{
	return m_caption;
}

const OpacityContainer& v4PostZoneDataBC::defaultOpacity() const
{
	return m_defaultOpacity;
}

const QColor& v4PostZoneDataBC::defaultColor() const
{
	return m_defaultColor;
}

bool v4PostZoneDataBC::defaultShowName() const
{
	return m_defaultShowName;
}

int v4PostZoneDataBC::defaultPointSize() const
{
	return m_defaultPointSize;
}

SolverDefinitionBoundaryCondition* v4PostZoneDataBC::condition() const
{
	return m_condition;
}

void v4PostZoneDataBC::setupIndicesAndEdges(const iRICLib::H5CgnsZone& zone, const std::vector<int>& indices)
{
	if (zone.type() == iRICLib::H5CgnsZone::Type::Unstructured) {
		// unstructured grid.
		if (m_condition->position() == SolverDefinitionBoundaryCondition::pNode ||
			m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
			m_indices.clear();
			for (int i = 0; i < static_cast<int> (indices.size()); ++i) {
				m_indices.insert(indices[i] - 1);
			}
		}
		else if (m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
			for (int i = 0; i < static_cast<int>(indices.size()) / 2; ++i) {
				Edge e(indices[i * 2] - 1, indices[i * 2 + 1] - 1);
				m_edges.insert(e);
			}
		}
	}
	else {
		// structured grid.
		m_indices.clear();
		if (m_condition->position() == SolverDefinitionBoundaryCondition::pNode) {
			for (int idx = 0; idx < static_cast<int> (indices.size()) / 2; ++idx) {
				int i = indices[idx * 2];
				int j = indices[idx * 2 + 1];
				m_indices.insert(pointIndex(zone, i - 1, j - 1));
			}
		}
		else if (m_condition->position() == SolverDefinitionBoundaryCondition::pCell) {
			for (int idx = 0; idx < static_cast<int> (indices.size()) / 2; ++idx) {
				int i = indices[idx * 2];
				int j = indices[idx * 2 + 1];
				m_indices.insert(cellIndex(zone, i - 1, j - 1));
			}
		}
		else if (m_condition->position() == SolverDefinitionBoundaryCondition::pEdge) {
			for (int idx = 0; idx < static_cast<int> (indices.size()) / 4; ++idx) {
				int i1 = indices[idx * 4];
				int j1 = indices[idx * 4 + 1];
				int i2 = indices[idx * 4 + 2];
				int j2 = indices[idx * 4 + 3];
				Edge e(pointIndex(zone, i1 - 1, j1 - 1), pointIndex(zone, i2 - 1, j2 - 1));
				m_edges.insert(e);
			}
		}
	}
}

int v4PostZoneDataBC::pointIndex(const iRICLib::H5CgnsZone& zone, int i, int j)
{
	if (nodeDims.size() == 0) {
		zone.nodeDims(&nodeDims);
	}
	return i + j * nodeDims.at(0);
}

int v4PostZoneDataBC::cellIndex(const iRICLib::H5CgnsZone& zone, int i, int j)
{
	if (cellDims.size() == 0) {
		zone.cellDims(&cellDims);
	}
	return i + j * cellDims.at(0);
}
