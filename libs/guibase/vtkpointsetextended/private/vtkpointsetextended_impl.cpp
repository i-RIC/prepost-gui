#include "vtkpointsetextended_impl.h"

#include <guibase/vtktool/vtkpointsetgeos2dindex.h>

#include <vtkAbstractPointLocator.h>

vtkPointSetExtended::Impl::Impl(vtkPointSet* data, vtkPointSetExtended* p) :
	m_data {data},
	m_pointLocator {nullptr},
	m_cellIndex {nullptr},
	m_valueRangeSet {},
	m_parent {p}
{}

vtkPointSetExtended::Impl::~Impl()
{
	delete m_cellIndex;

	if (m_pointLocator != nullptr) {
		m_pointLocator->Delete();
	}

	if (m_data != nullptr) {
		m_data->Delete();
	}
}

void vtkPointSetExtended::Impl::rebuildCellIndex()
{
	delete m_cellIndex;
	m_cellIndex = new vtkPointSetGeos2dIndex(m_data);
}
