#ifndef GEODATABACKGROUNDSTRINGCREATOR_H
#define GEODATABACKGROUNDSTRINGCREATOR_H

#include "geodatabackgroundcreatort.h"

#include <vtkStringArray.h>

class GeoDataBackgroundStringCreator : public GeoDataBackgroundCreatorT<std::string, vtkStringArray>
{

private:
	GeoDataBackgroundStringCreator();

public:
	static GeoDataBackgroundStringCreator* instance();

protected:
	static GeoDataBackgroundStringCreator* m_instance;
};

#endif // GEODATABACKGROUNDSTRINGCREATOR_H
