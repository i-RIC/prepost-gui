#ifndef GEODATABACKGROUNDREALCREATOR_H
#define GEODATABACKGROUNDREALCREATOR_H

#include "geodatabackgroundcreatort.h"
#include <vtkDoubleArray.h>

class GeoDataBackgroundRealCreator : public GeoDataBackgroundCreatorT<double, vtkDoubleArray>
{

private:
	GeoDataBackgroundRealCreator();

public:
	static GeoDataBackgroundRealCreator* instance();

protected:
	static GeoDataBackgroundRealCreator* m_instance;
};

#endif // GEODATABACKGROUNDREALCREATOR_H
