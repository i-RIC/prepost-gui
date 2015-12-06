#ifndef GRID_IMPL_H
#define GRID_IMPL_H

#include "../grid.h"

#include <QList>
#include <QMap>

#include <vtkAlgorithm.h>
#include <vtkSmartPointer.h>

class Grid::Impl
{
public:
	Impl(vtkPointSet* ps, const std::string& zoneName, SolverDefinitionGridType::GridType gt);
	~Impl();

	SolverDefinitionGridType::GridType m_gridType;
	std::string m_zoneName;

	vtkPointSet* m_vtkGrid;

	QList<GridAttributeContainer*> m_gridAttributes;
	QMap<std::string, GridAttributeContainer*> m_gridAttributeNameMap;

	vtkSmartPointer<vtkAlgorithm> m_vtkFilteredShapeAlgorithm;
	vtkSmartPointer<vtkAlgorithm> m_vtkFilteredPointsAlgorithm;
	vtkSmartPointer<vtkAlgorithm> m_vtkFilteredCellsAlgorithm;

	bool m_isModified;
	bool m_isMasked;
};

#endif // GRID_IMPL_H
