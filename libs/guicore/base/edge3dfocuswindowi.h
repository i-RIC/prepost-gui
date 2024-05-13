#ifndef EDGE3DFOCUSWINDOWI_H
#define EDGE3DFOCUSWINDOWI_H

class Edge3dFocusWindowI
{
public:
	virtual ~Edge3dFocusWindowI() {}

	virtual void setEdgeFocus(const std::string& zoneName, vtkIdType i, vtkIdType j, vtkIdType k) = 0;
	virtual void clearEdgeFocus() = 0;
};

#endif // EDGE3DFOCUSWINDOWI_H
