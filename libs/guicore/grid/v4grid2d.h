#ifndef V4GRID2D_H
#define V4GRID2D_H

#include "../guicore_global.h"

#include "v4grid.h"

class GUICOREDLL_EXPORT v4Grid2d : public v4Grid
{
public:
	v4Grid2d(vtkPointSetExtended* data);
	~v4Grid2d();

	vtkPointSet* vtkFilteredData() const;
	vtkPointSet* vtkFilteredIndexData() const;

	QPointF point2d(vtkIdType index) const;
	void setPoint2d(vtkIdType index, const QPointF& v);

	bool isMasked() const;
	void setMasked(bool masked);

	virtual void updateFilteredData(double xMin, double xMax, double yMin, double yMax) = 0;

protected:
	void setFilteredData(vtkPointSet* data);
	void setFilteredIndexData(vtkPointSet* data);

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

#endif // V4GRID2D_H
