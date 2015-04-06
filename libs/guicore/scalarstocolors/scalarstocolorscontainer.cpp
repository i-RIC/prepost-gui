#include "scalarstocolorscontainer.h"
#include <vtkScalarsToColors.h>

ScalarsToColorsContainer::~ScalarsToColorsContainer()
{
	m_vtkObj->Delete();
	m_vtkDarkObj->Delete();
}
