#ifndef POST2DWINDOWIEDGESCALARGROUPDATAITEM_H
#define POST2DWINDOWIEDGESCALARGROUPDATAITEM_H

#include "post2dwindowabstractcellscalargroupdataitem.h"

class Post2dWindowIEdgeScalarGroupDataItem : public Post2dWindowAbstractCellScalarGroupDataItem
{
public:
	Post2dWindowIEdgeScalarGroupDataItem(const std::string& target, Post2dWindowDataItem* parent);

private:
	void getDimensions(int* dimI, int* dimJ) const override;
	v4SolutionGrid::Position position() const override;
	vtkPointSet* buildRegionFilteredData() const override;
	const ValueRangeContainer& valueRange(const::std::string& name) const override;
	vtkPointSetExtended* data() const override;
	vtkPointSet* filteredData() const override;
};

#endif // POST2DWINDOWIEDGESCALARGROUPDATAITEM_H
