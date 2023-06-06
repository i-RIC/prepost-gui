#ifndef GRID_IMPL_H
#define GRID_IMPL_H

#include "../grid.h"

#include <QList>
#include <QMap>

class vtkAbstractPointLocator;

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

	vtkPointSet* m_vtkFilteredGrid;
	vtkPointSet* m_vtkFilteredIndexGrid;

	vtkAbstractPointLocator* m_pointLocator;

	PreProcessorGridDataItemInterface* m_dataItem;

	bool m_isModified;
	bool m_isMasked;
};

#endif // GRID_IMPL_H
