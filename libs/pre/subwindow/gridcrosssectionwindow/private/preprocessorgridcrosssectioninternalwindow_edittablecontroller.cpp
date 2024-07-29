#include "preprocessorgridcrosssectioninternalwindow_edittablecontroller.h"
#include "preprocessorgridcrosssectioninternalwindow_edittablecontroller_delegate.h"

PreProcessorGridCrosssectionInternalWindow::EditTableController::EditTableController(QTableView* view, Impl* impl) :
	AbstractCrosssectionWindow::EditTableController(view, impl)
{
	delete m_delegate;
	m_delegate = new Delegate(this);

	view->setItemDelegate(m_delegate);
}
