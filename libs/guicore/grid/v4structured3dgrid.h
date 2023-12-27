#ifndef V4STRUCTURED3DGRID_H
#define V4STRUCTURED3DGRID_H

#include "../guicore_global.h"

#include "v4grid3dt.h"

#include <guibase/vtkpointsetextended/vtkstructuredgridextended3d.h>

class vtkPolyDataExtended3d;

class GUICOREDLL_EXPORT v4Structured3dGrid : public v4Grid3dT<vtkStructuredGridExtended3d>
{
public:
	v4Structured3dGrid();
	~v4Structured3dGrid();

	void clear() override;
	void updateValueRangeSet() override;
	vtkPointSet* vtkIndexData() const override;

	void setDimensions(vtkIdType dimI, vtkIdType dimJ, vtkIdType dimK);
	void getDimensions(vtkIdType* dimI, vtkIdType* dimJ, vtkIdType* dimK) const;
	vtkIdType dimensionI() const;
	vtkIdType dimensionJ() const;
	vtkIdType dimensionK() const;

	vtkIdType pointIndex(vtkIdType i, vtkIdType j, vtkIdType k) const;
	void getPointIJKIndex(vtkIdType index, vtkIdType* i, vtkIdType* j, vtkIdType* k) const;

	vtkIdType cellIndex(vtkIdType i, vtkIdType j, vtkIdType k) const;
	void getCellIJKIndex(vtkIdType index, vtkIdType* i, vtkIdType* j, vtkIdType* k) const;

	vtkIdType iFaceIndex(vtkIdType i, vtkIdType j, vtkIdType k) const;
	void getIFaceIJKIndex(vtkIdType index, vtkIdType* i, vtkIdType* j, vtkIdType* k) const;

	vtkIdType jFaceIndex(vtkIdType i, vtkIdType j, vtkIdType k) const;
	void getJFaceIJKIndex(vtkIdType index, vtkIdType* i, vtkIdType* j, vtkIdType* k) const;

	vtkIdType kFaceIndex(vtkIdType i, vtkIdType j, vtkIdType k) const;
	void getKFaceIJKIndex(vtkIdType index, vtkIdType* i, vtkIdType* j, vtkIdType* k) const;

	vtkPolyDataExtended3d* vtkIFaceData() const;
	vtkPolyDataExtended3d* vtkJFaceData() const;
	vtkPolyDataExtended3d* vtkKFaceData() const;

private:
	class Impl;
	std::unique_ptr<Impl> impl;
};

#endif // V4STRUCTURED3DGRID_H
