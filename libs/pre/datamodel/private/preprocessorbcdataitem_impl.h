#ifndef PREPROCESSORBCDATAITEM_IMPL_H
#define PREPROCESSORBCDATAITEM_IMPL_H

#include "../preprocessorbcdataitem.h"

#include <misc/opacitycontainer.h>

class BoundaryConditionDialog;

class vtkActor;
class vtkActor2D;
class vtkPolyData;
class vtkPolyDataMapper;
class vtkTextMapper;

class PreProcessorBCDataItem::Impl
{
public:
	Impl(PreProcessorBCDataItem* item);
	~Impl();

	int m_projectNumber;
	int m_cgnsNumber;

	OpacityContainer m_opacity;
	bool m_mapped;
	bool m_definingBoundingBox;
	bool m_isCustomModified;

	bool m_hideSetting;

	QSet<vtkIdType> m_indices;
	QSet<Edge> m_edges;

	BoundaryConditionDialog* m_dialog;
	SolverDefinitionBoundaryCondition* m_condition;

	vtkPolyData* m_data;
	vtkPolyDataMapper* m_mapper;
	vtkActor* m_actor;

	vtkActor2D* m_nameActor;
	vtkTextMapper* m_nameMapper;

	QAction* m_editAction;
	QAction* m_assignAction;
	QAction* m_releaseAction;
};

#endif // PREPROCESSORBCDATAITEM_IMPL_H
