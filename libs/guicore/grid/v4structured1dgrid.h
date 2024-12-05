#ifndef V4STRUCTURED1DGRID_H
#define V4STRUCTURED1DGRID_H

#include "../guicore_global.h"

#include "v4grid1dt.h"

#include <guibase/vtkpointsetextended/vtkstructuredgridextended2d.h>

#include <memory>

class GUICOREDLL_EXPORT v4Structured1dGrid : public v4Grid1dT<vtkStructuredGridExtended2d>
{
public:
	v4Structured1dGrid();
	~v4Structured1dGrid();

	void setDimension(vtkIdType dim);
	vtkIdType dimension() const;

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

#endif // V4STRUCTURED1DGRID_H
