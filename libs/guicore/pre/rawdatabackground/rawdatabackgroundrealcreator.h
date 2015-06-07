#ifndef RAWDATABACKGROUNDREALCREATOR_H
#define RAWDATABACKGROUNDREALCREATOR_H

#include "rawdatabackgroundcreatort.h"
#include <vtkDoubleArray.h>

class RawDataBackgroundRealCreator : public RawDataBackgroundCreatorT<double, vtkDoubleArray>
{

private:
	RawDataBackgroundRealCreator()
		: RawDataBackgroundCreatorT<double, vtkDoubleArray>() {
		m_typeName = "realBackground";
	}

public:
	static RawDataBackgroundRealCreator* instance() {
		if (! m_instance) {
			m_instance = new RawDataBackgroundRealCreator();
		}
		return m_instance;
	}

protected:
	static RawDataBackgroundRealCreator* m_instance;
};

#endif // RAWDATABACKGROUNDREALCREATOR_H
