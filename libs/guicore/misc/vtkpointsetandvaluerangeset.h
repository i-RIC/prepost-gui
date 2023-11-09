#ifndef VTKPOINTSETANDVALUERANGESET_H
#define VTKPOINTSETANDVALUERANGESET_H

#include "../guicore_global.h"

#include <guibase/vtktool/vtkpointsetvaluerangeset.h>

class vtkPointSet;

class GUICOREDLL_EXPORT vtkPointSetAndValueRangeSet
{
public:
	vtkPointSetAndValueRangeSet(vtkPointSet* data);
	virtual ~vtkPointSetAndValueRangeSet();

	vtkPointSet* data() const;
	const vtkPointSetValueRangeSet& valueRangeSet() const;

	void updateValueRangeSet();

protected:
	vtkPointSet* m_data;

private:
	vtkPointSetValueRangeSet m_valueRangeSet;
};

#endif // VTKPOINTSETANDVALUERANGESET_H
