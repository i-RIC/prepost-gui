#ifndef ABSTRACTCROSSSECTIONWINDOW_IMPL_STRUCTUREDGRIDBUILDERFORUNSTRUCTUREDEDGE_H
#define ABSTRACTCROSSSECTIONWINDOW_IMPL_STRUCTUREDGRIDBUILDERFORUNSTRUCTUREDEDGE_H

#include "abstractcrosssectionwindow_impl.h"

class AbstractCrosssectionWindow::Impl::StructuredGridBuilderForUnstructuredEdge
{
public:
	StructuredGridBuilderForUnstructuredEdge(Impl* impl);

	void build();

private:
	void copyNodeAttributes(v4Grid* src, v4Grid* tgt, const std::vector<vtkIdType>& nodeIds);
	void copyCellAttributes(v4Grid* src, v4Grid* tgt, const std::vector<vtkIdType>& cellIds);

	void copyAttributes(vtkDataSetAttributes* srcAtts, vtkDataSetAttributes* tgtAtts, const std::vector<vtkIdType>& Ids);

	Impl* m_impl;
};

#endif // ABSTRACTCROSSSECTIONWINDOW_IMPL_STRUCTUREDGRIDBUILDERFORUNSTRUCTUREDEDGE_H
