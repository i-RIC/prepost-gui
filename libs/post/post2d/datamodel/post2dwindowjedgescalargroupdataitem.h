#ifndef POST2DWINDOWJEDGESCALARGROUPDATAITEM_H
#define POST2DWINDOWJEDGESCALARGROUPDATAITEM_H

#include "post2dwindowabstractcellscalargroupdataitem.h"

class Post2dWindowJEdgeScalarGroupDataItem : public Post2dWindowAbstractCellScalarGroupDataItem
{
public:
	Post2dWindowJEdgeScalarGroupDataItem(const std::string& target, Post2dWindowDataItem* parent);

private:
	void getDimensions(int* dimI, int* dimJ) const override;
	v4SolutionGrid::Position position() const override;
	vtkPointSet* buildRegionFilteredData() const override;
	const ValueRangeContainer& valueRange(const::std::string& name) const override;
	vtkPointSetExtended* data() const override;
	vtkPointSet* filteredData() const override;
};

#endif // POST2DWINDOWJEDGESCALARGROUPDATAITEM_H
