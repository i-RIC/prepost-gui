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

v4Grid* PreProcessorGridCrosssectionInternalWindow::grid()
{
	auto g = m_item->grid();
	if (g == nullptr) {return nullptr;}

	return g->grid();
}

void PreProcessorGridCrosssectionInternalWindow::updateEdgeFocus()
{
	auto zoneName = m_item->gridAndGridCreatingConditionDataItem()->zoneName();

	if (mode() == Mode::StructuredIJ) {
		vtkIdType i = 0, j = 0;
		auto c = controller();
		if (c->targetDirection() == Direction::I) {
			i = c->targetIndex();
			j = -1;
		} else if (c->targetDirection() == Direction::J) {
			i = -1;
			j = c->targetIndex();
		}
		if (i == -1 && j == -1) {return;}

		m_item->iricMainWindow()->setEdgeFocus(zoneName, i, j);
	} else if (mode() == Mode::UnstructuredEdge) {
		m_item->iricMainWindow()->setEdgeFocus(zoneName, targetLine());
	}
}

ColorMapSettingContainerI* PreProcessorGridCrosssectionInternalWindow::preColorMapSetting(const std::string& name) const
{
	return m_item->gridTypeDataItem()->colorMapSetting(name);
}
