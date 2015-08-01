#include "geodatabackgroundrealcreator.h"

GeoDataBackgroundRealCreator* GeoDataBackgroundRealCreator::m_instance = nullptr;

GeoDataBackgroundRealCreator::GeoDataBackgroundRealCreator() :
	GeoDataBackgroundCreatorT<double, vtkDoubleArray> {}
{
	m_typeName = "realBackground";
}

GeoDataBackgroundRealCreator* GeoDataBackgroundRealCreator::instance()
{
	if (! m_instance) {
		m_instance = new GeoDataBackgroundRealCreator();
	}
	return m_instance;
}
