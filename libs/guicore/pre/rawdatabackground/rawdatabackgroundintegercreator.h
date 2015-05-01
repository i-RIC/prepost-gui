#ifndef RAWDATABACKGROUNDINTEGERCREATOR_H
#define RAWDATABACKGROUNDINTEGERCREATOR_H

#include "rawdatabackgroundcreatort.h"
#include <vtkIntArray.h>

class RawDataBackgroundIntegerCreator : public RawDataBackgroundCreatorT<int, vtkIntArray>
{
private:
	RawDataBackgroundIntegerCreator()
		: RawDataBackgroundCreatorT<int, vtkIntArray>() {
		m_typeName = "integerBackground";
	}
public:
	static RawDataBackgroundIntegerCreator* instance() {
		if (! m_instance) {
			m_instance = new RawDataBackgroundIntegerCreator();
		}
		return m_instance;
	}
protected:
	static RawDataBackgroundIntegerCreator* m_instance;
};

#endif // RAWDATABACKGROUNDINTEGERCREATOR_H
