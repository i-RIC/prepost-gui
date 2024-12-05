#ifndef V4GRID1DT_H
#define V4GRID1DT_H

#include "v4grid1d.h"

template <typename V>
class v4Grid1dT : public v4Grid1d
{
public:
	v4Grid1dT();

	V* vtkConcreteData() const;
};

#include "private/v4grid1dt_detail.h"

#endif // V4GRID1DT_H
