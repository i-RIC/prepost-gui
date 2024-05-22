#ifndef EDGE2DFOCUSWINDOWI_H
#define EDGE2DFOCUSWINDOWI_H

#include <vtkIdList.h>

class Edge2dFocusWindowI
{
public:
	virtual ~Edge2dFocusWindowI() {}

	virtual void setEdgeFocus(const std::string& zoneName, vtkIdType i, vtkIdType j) = 0;
	virtual void clearEdgeFocus() = 0;
};

#endif // EDGE2DFOCUSWINDOWI_H
