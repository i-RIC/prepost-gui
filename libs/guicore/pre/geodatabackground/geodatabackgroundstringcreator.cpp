#include "../../solverdef/solverdefinitiongridattributet.h"
#include "geodatabackground.h"
#include "geodatabackgroundstringcreator.h"

GeoDataBackgroundStringCreator* GeoDataBackgroundStringCreator::m_instance = nullptr;

GeoDataBackgroundStringCreator::GeoDataBackgroundStringCreator() :
	GeoDataBackgroundCreatorT<std::string, vtkStringArray> {"stringBackground"}
{}

GeoDataBackgroundStringCreator* GeoDataBackgroundStringCreator::instance()
{
	if (! m_instance) {
		m_instance = new GeoDataBackgroundStringCreator();
	}
	return m_instance;
}
