#ifndef GEODATABACKGROUNDREALCREATOR_H
#define GEODATABACKGROUNDREALCREATOR_H

#include "geodatabackgroundcreatort.h"
#include <vtkDoubleArray.h>

class GeoDataBackgroundRealCreator : public GeoDataBackgroundCreatorT<double, vtkDoubleArray>
{

private:
	GeoDataBackgroundRealCreator()
		: GeoDataBackgroundCreatorT<double, vtkDoubleArray>() {
		m_typeName = "realBackground";
	}

public:
	static GeoDataBackgroundRealCreator* instance() {
		if (! m_instance) {
			m_instance = new GeoDataBackgroundRealCreator();
		}
		return m_instance;
	}

protected:
	static GeoDataBackgroundRealCreator* m_instance;
};

#endif // GEODATABACKGROUNDREALCREATOR_H
