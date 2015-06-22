#ifndef GEODATABACKGROUNDINTEGERCREATOR_H
#define GEODATABACKGROUNDINTEGERCREATOR_H

#include "geodatabackgroundcreatort.h"
#include <vtkIntArray.h>

class GeoDataBackgroundIntegerCreator : public GeoDataBackgroundCreatorT<int, vtkIntArray>
{

private:
	GeoDataBackgroundIntegerCreator()
		: GeoDataBackgroundCreatorT<int, vtkIntArray>() {
		m_typeName = "integerBackground";
	}

public:
	static GeoDataBackgroundIntegerCreator* instance() {
		if (! m_instance) {
			m_instance = new GeoDataBackgroundIntegerCreator();
		}
		return m_instance;
	}

protected:
	static GeoDataBackgroundIntegerCreator* m_instance;
};

#endif // GEODATABACKGROUNDINTEGERCREATOR_H
