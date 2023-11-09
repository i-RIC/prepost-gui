#ifndef POST2DWINDOWCELLSCALARGROUPDATAITEM_H
#define POST2DWINDOWCELLSCALARGROUPDATAITEM_H

#include "post2dwindowabstractcellscalargroupdataitem.h"

class Post2dWindowCellScalarGroupDataItem : public Post2dWindowAbstractCellScalarGroupDataItem
{
public:
	Post2dWindowCellScalarGroupDataItem(const std::string& target, Post2dWindowDataItem* parent);
	QDialog* propertyDialog(QWidget* parent) override;

private:
	void getDimensions(int* dimI, int* dimJ) const override;
	v4SolutionGrid::Position position() const override;
	vtkPointSet* buildRegionFilteredData() const override;
	const ValueRangeContainer& valueRange(const::std::string& name) const override;
	vtkPointSetExtended* data() const override;
	vtkPointSet* filteredData() const override;
};

#endif // POST2DWINDOWCELLSCALARGROUPDATAITEM_H
