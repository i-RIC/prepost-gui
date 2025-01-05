#include "preprocessorgridcrosssectioninternalwindow.h"
#include "preprocessorgridcrosssectionwindow.h"
#include "private/preprocessorgridcrosssectioninternalwindow_edittablecontroller.h"
#include "../../datamodel/preprocessorgridandgridcreatingconditiondataitem.h"
#include "../../datamodel/preprocessorgriddataitem.h"
#include "../../datamodel/preprocessorgridtypedataitem.h"

#include <crosssection/public/abstractcrosssectionwindow_controller.h>
#include <crosssection/public/abstractcrosssectionwindow_graphicsview.h>
#include <crosssection/private/abstractcrosssectionwindow_impl.h>
#include <guicore/base/iricmainwindowi.h>
#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>

PreProcessorGridCrosssectionInternalWindow::PreProcessorGridCrosssectionInternalWindow(PreProcessorGridDataItem* item, QWidget *parent) :
	AbstractCrosssectionWindow {parent},
	m_item {item}
{
	delete impl->m_editTableController;
	impl->m_editTableController = new EditTableController(editTable(), impl);

	impl->graphicsView()->setEditEnabled(true);
	impl->graphicsView()->setGridDataItem(item);
}

PreProcessorGridCrosssectionInternalWindow::~PreProcessorGridCrosssectionInternalWindow()
{}

QMdiSubWindow* PreProcessorGridCrosssectionInternalWindow::mdiSubWindow() const
{
	auto w = dynamic_cast<PreProcessorGridCrosssectionWindow*> (parentWidget());
	return dynamic_cast<QMdiSubWindow*> (w->parentWidget());
}

v4Structured2dGrid* PreProcessorGridCrosssectionInternalWindow::grid()
{
	auto g = m_item->grid();
	if (g == nullptr) {return nullptr;}

	return dynamic_cast<v4Structured2dGrid*> (g->grid());
}

void PreProcessorGridCrosssectionInternalWindow::updateEdgeFocus()
{
	auto c = controller();
	auto ti = c->targetIndex();
	if (ti == -1) {return;}

	auto zoneName = m_item->gridAndGridCreatingConditionDataItem()->zoneName();
	vtkIdType i = 0, j = 0;
	if (c->targetDirection() == Direction::I) {
		i = ti;
		j = -1;
	} else if (c->targetDirection() == Direction::J) {
		i = -1;
		j = ti;
	}

	m_item->iricMainWindow()->setEdgeFocus(zoneName, i, j);
}

ColorMapSettingContainerI* PreProcessorGridCrosssectionInternalWindow::preColorMapSetting(const std::string& name) const
{
	return m_item->gridTypeDataItem()->colorMapSetting(name);
}
