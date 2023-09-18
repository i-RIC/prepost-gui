#ifndef V4GRID2D_IMPL_H
#define V4GRID2D_IMPL_H

#include "../v4grid2d.h"

class v4Grid2d::Impl
{
public:
	Impl();
	~Impl();

	vtkPointSet* m_vtkFilteredData;
	vtkPointSet* m_vtkFilteredIndexData;
	bool m_isMasked;
};

#endif // V4GRID2D_IMPL_H
