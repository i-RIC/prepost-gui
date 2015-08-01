#include "scalarstocolorscontainer.h"

#include <vtkScalarsToColors.h>

ScalarsToColorsContainer::ScalarsToColorsContainer(ProjectDataItem* d) :
	ProjectDataItem {d},
	m_vtkObj {nullptr},
	m_vtkDarkObj {nullptr}
{}

ScalarsToColorsContainer::~ScalarsToColorsContainer()
{
	m_vtkObj->Delete();
	m_vtkDarkObj->Delete();
}

vtkScalarsToColors* ScalarsToColorsContainer::vtkObj() const
{
	return m_vtkObj;
}

vtkScalarsToColors* ScalarsToColorsContainer::vtkDarkObj() const
{
	return m_vtkDarkObj;
}

void ScalarsToColorsContainer::setValueRange(double, double)
{}

void ScalarsToColorsContainer::getValueRange(double*, double* ) const
{}
