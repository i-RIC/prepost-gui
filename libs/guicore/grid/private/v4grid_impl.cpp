#include "../v4gridstructurecheckeri.h"
#include "../public/v4grid_attributedataprovider.h"
#include "v4grid_impl.h"

#include <guibase/vtkpointsetextended/vtkpointsetextended.h>

v4Grid::Impl::Impl(vtkPointSetExtended* data) :
	m_vtkData {data},
	m_structureChecker {new v4GridStructureCheckerI {}},
	m_attributeDataProvider {nullptr}
{}

v4Grid::Impl::~Impl()
{
	delete m_vtkData;
	delete m_structureChecker;
	delete m_attributeDataProvider;
}
