#ifndef V4POLYDATA2D_H
#define V4POLYDATA2D_H

#include "../guicore_global.h"

#include "v4grid2dt.h"

#include <guibase/vtkpointsetextended/vtkpolydataextended2d.h>

class GUICOREDLL_EXPORT v4PolyData2d : public v4Grid2dT<vtkPolyDataExtended2d>
{
public:
	v4PolyData2d();

	const std::vector<vtkIdType>& cellIds() const;
	void setCellIds(const std::vector<vtkIdType>& ids);

private:
	void updateFilteredData(double xMin, double xMax, double yMin, double yMax) override;

	std::vector<vtkIdType> m_cellIds;
};

#endif // V4POLYDATA2D_H
