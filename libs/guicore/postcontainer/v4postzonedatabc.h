#ifndef V4POSTZONEDATABC_H
#define V4POSTZONEDATABC_H

#include "../guicore_global.h"

#include <misc/edge.h>
#include <misc/opacitycontainer.h>

#include <QSet>
#include <QColor>

#include <vtkIdList.h>
#include <hdf5.h>

#include <unordered_set>
#include <string>

namespace iRICLib {
	class H5CgnsBc;
	class H5CgnsZone;
}

class SolverDefinitionBoundaryCondition;

class GUICOREDLL_EXPORT v4PostZoneDataBC
{
public:
	v4PostZoneDataBC(SolverDefinitionBoundaryCondition* condition, const iRICLib::H5CgnsZone& zone, const iRICLib::H5CgnsBc& bc);
	const std::unordered_set<vtkIdType>& indices() const;
	const QSet<Edge>& edges() const;
	const std::string& caption() const;

	const OpacityContainer& defaultOpacity() const;
	const QColor& defaultColor() const;
	bool defaultShowName() const;
	int defaultPointSize() const;

	SolverDefinitionBoundaryCondition* condition() const;

private:
	void setupIndicesAndEdges(const iRICLib::H5CgnsZone& zone, const std::vector<int>& indices);
	int pointIndex(const iRICLib::H5CgnsZone& zone, int i, int j);
	int cellIndex(const iRICLib::H5CgnsZone& zone, int i, int j);

	std::unordered_set<vtkIdType> m_indices;
	QSet<Edge> m_edges;
	std::string m_caption;

	OpacityContainer m_defaultOpacity;
	QColor m_defaultColor;
	bool m_defaultShowName;
	int m_defaultPointSize;

	SolverDefinitionBoundaryCondition* m_condition;

	std::vector<hsize_t> nodeDims;
	std::vector<hsize_t> cellDims;
};

#endif // V4POSTZONEDATABC_H
