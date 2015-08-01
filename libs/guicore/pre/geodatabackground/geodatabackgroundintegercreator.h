#ifndef GEODATABACKGROUNDINTEGERCREATOR_H
#define GEODATABACKGROUNDINTEGERCREATOR_H

#include "geodatabackgroundcreatort.h"
#include <vtkIntArray.h>

class GeoDataBackgroundIntegerCreator : public GeoDataBackgroundCreatorT<int, vtkIntArray>
{

private:
	GeoDataBackgroundIntegerCreator();

public:
	static GeoDataBackgroundIntegerCreator* instance();

protected:
	static GeoDataBackgroundIntegerCreator* m_instance;
};

#endif // GEODATABACKGROUNDINTEGERCREATOR_H
