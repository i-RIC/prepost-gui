#include "preprocessorgridcrosssectioninternalwindow.h"
#include "../../datamodel/preprocessorgridandgridcreatingconditiondataitem.h"
#include "../../datamodel/preprocessorgriddataitem.h"
#include "../../datamodel/preprocessorgridtypedataitem.h"

#include <crosssection/public/abstractcrosssectionwindow_controller.h>
#include <guicore/base/iricmainwindowi.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>

PreProcessorGridCrosssectionInternalWindow::PreProcessorGridCrosssectionInternalWindow(PreProcessorGridDataItem* item, QWidget *parent) :
	AbstractCrosssectionWindow {parent},
	m_item {item}
{}

PreProcessorGridCrosssectionInternalWindow::~PreProcessorGridCrosssectionInternalWindow()
{}

v4Structured2dGrid* PreProcessorGridCrosssectionInternalWindow::grid()
{
	return dynamic_cast<v4Structured2dGrid*> (m_item->grid()->grid());
}

void PreProcessorGridCrosssectionInternalWindow::updateEdgeFocus()
{
	auto zoneName = m_item->gridAndGridCreatingConditionDataItem()->zoneName();
	vtkIdType i = 0, j = 0;
	auto c = controller();
	if (c->targetDirection() == Direction::I) {
		i = -1;
		j = c->targetIndex();
	} else if (c->targetDirection() == Direction::J) {
		i = c->targetIndex();
		j = -1;
	}

	m_item->iricMainWindow()->setEdgeFocus(zoneName, i, j);
}

ColorMapSettingContainerI* PreProcessorGridCrosssectionInternalWindow::preColorMapSetting(const std::string& name) const
{
	return m_item->gridTypeDataItem()->colorMapSetting(name);
}
