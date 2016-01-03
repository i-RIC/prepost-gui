#include "../../solverdef/solverdefinitiongridattributet.h"
#include "geodatabackground.h"
#include "geodatabackgroundintegercreator.h"

GeoDataBackgroundIntegerCreator* GeoDataBackgroundIntegerCreator::m_instance = nullptr;

GeoDataBackgroundIntegerCreator::GeoDataBackgroundIntegerCreator() :
	GeoDataBackgroundCreatorT<int, vtkIntArray> {"integerBackground"}
{}

GeoDataBackgroundIntegerCreator* GeoDataBackgroundIntegerCreator::instance()
{
	if (! m_instance) {
		m_instance = new GeoDataBackgroundIntegerCreator();
	}
	return m_instance;
}
