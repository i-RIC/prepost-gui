#include "vtkpointsetextended_impl.h"

#include <guibase/vtktool/vtkpointsetgeos2dindex.h>

#include <vtkStaticPointLocator.h>
#include <vtkStaticPointLocator2D.h>

vtkPointSetExtended::Impl::Impl(bool twoDimensional, bool geosIndex, vtkPointSet* data, vtkPointSetExtended* p) :
	m_twoDimensional {twoDimensional},
	m_geosIndex {geosIndex},
	m_data {data},
	m_pointLocator {nullptr},
	m_cellIndex {nullptr},
	m_valueRangeSet {},
	m_parent {p}
{}

vtkPointSetExtended::Impl::~Impl()
{
	deleteCellIndex();

	if (m_pointLocator != nullptr) {
		m_pointLocator->Delete();
	}

	if (m_data != nullptr) {
		m_data->Delete();
	}
}

void vtkPointSetExtended::Impl::deleteCellIndex()
{
	delete m_cellIndex;
	m_cellIndex = nullptr;
}

void vtkPointSetExtended::Impl::buildCellIndexIfNotExists()
{
	if (m_cellIndex != nullptr) {return;}

	if (m_geosIndex) {
		m_cellIndex = new vtkPointSetGeos2dIndex(m_data);
	}
}

void vtkPointSetExtended::Impl::buildPointLocatorIfNotExists()
{
	if (m_pointLocator != nullptr) {return;}

	if (m_twoDimensional) {
		m_pointLocator = vtkStaticPointLocator2D::New();
	} else {
		m_pointLocator = vtkStaticPointLocator::New();
	}
	m_pointLocator->SetDataSet(m_data);

	if (m_data->GetNumberOfPoints() > 0) {
		m_pointLocator->BuildLocator();
	}
}
