#include "post2dwindowabstractcellscalargrouptopdataitem.h"
#include "post2dwindowcellscalargroupdataitem.h"
#include "post2dwindowgridtypedataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowabstractcellscalargroupdataitem_impl.h"
#include "private/post2dwindowabstractcellscalargroupdataitem_setting.h"
#include "private/post2dwindowabstractcellscalargroupdataitem_settingeditwidget.h"

#include <guicore/datamodel/graphicswindowdataitemupdateactorsettingdialog.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/grid/v4unstructured2dgrid.h>
#include <guicore/postcontainer/v4postzonedatacontainer.h>
#include <guicore/postcontainer/v4solutiongrid.h>

Post2dWindowCellScalarGroupDataItem::Post2dWindowCellScalarGroupDataItem(const std::string& target, Post2dWindowDataItem* parent) :
	Post2dWindowAbstractCellScalarGroupDataItem {target, parent}
{}

QDialog* Post2dWindowCellScalarGroupDataItem::propertyDialog(QWidget* parent)
{
	auto dialog = dynamic_cast<GraphicsWindowDataItemUpdateActorSettingDialog*> (Post2dWindowAbstractCellScalarGroupDataItem::propertyDialog(parent));
	auto widget = dynamic_cast<Post2dWindowAbstractCellScalarGroupDataItem::SettingEditWidget*> (dialog->widget());

	auto gridData = topDataItem()->zoneDataItem()->v4DataContainer()->gridData();
	auto grid = gridData->grid();

	if (dynamic_cast<v4Structured2dGrid*>(grid) != nullptr) {
		widget->hideLineWidth();
	} else {
		auto ugrid = dynamic_cast<v4Structured2dGrid*>(grid);
		if (ugrid != nullptr) {
			auto vgrid = ugrid->vtkConcreteData()->data();
			auto firstCell = vgrid->GetCell(0);
			if (firstCell != nullptr && firstCell->GetCellType() == VTK_TRIANGLE) {
				widget->hideLineWidth();
			}
		}
	}

	return dialog;
}

void Post2dWindowCellScalarGroupDataItem::getDimensions(int* dimI, int* dimJ) const
{
	auto sGrid = dynamic_cast<v4Structured2dGrid*> (topDataItem()->zoneDataItem()->v4DataContainer()->gridData()->grid());
	*dimI = sGrid->dimensionI() - 1;
	*dimJ = sGrid->dimensionJ() - 1;
}

v4SolutionGrid::Position Post2dWindowCellScalarGroupDataItem::position() const
{
	return v4SolutionGrid::Position::CellCenter;
}

vtkPointSet* Post2dWindowCellScalarGroupDataItem::buildRegionFilteredData() const
{
	auto grid = topDataItem()->zoneDataItem()->v4DataContainer()->gridData()->grid();
	return impl->m_setting.regionSetting.buildCellFilteredData(grid->vtkData()->data());
}

const ValueRangeContainer& Post2dWindowCellScalarGroupDataItem::valueRange(const::std::string& name) const
{
	return topDataItem()->zoneDataItem()->gridTypeDataItem()->cellValueRange(name);
}

vtkPointSetExtended* Post2dWindowCellScalarGroupDataItem::data() const
{
	auto cont = topDataItem()->zoneDataItem()->v4DataContainer();
	if (cont == nullptr) {return nullptr;}

	auto grid = cont->gridData();
	if (grid == nullptr) {return nullptr;}

	return grid->grid()->vtkData();
}

vtkPointSet* Post2dWindowCellScalarGroupDataItem::filteredData() const
{
	auto cont = topDataItem()->zoneDataItem()->v4DataContainer();
	if (cont == nullptr) {return nullptr;}

	auto grid = cont->gridData();
	if (grid == nullptr) {return nullptr;}

	auto grid2d = dynamic_cast<v4Grid2d*> (grid->grid());
	return grid2d->vtkFilteredData();
}
