#ifndef PREPROCESSORGRIDCROSSSECTIONINTERNALWINDOW_EDITTABLECONTROLLER_H
#define PREPROCESSORGRIDCROSSSECTIONINTERNALWINDOW_EDITTABLECONTROLLER_H

#include "../preprocessorgridcrosssectioninternalwindow.h"
#include <crosssection/private/abstractcrosssectionwindow_edittablecontroller.h>

class PreProcessorGridCrosssectionInternalWindow::EditTableController : public AbstractCrosssectionWindow::EditTableController
{
public:
	EditTableController(QTableView* view, Impl* impl);

private:
	class Delegate;
};

#endif // PREPROCESSORGRIDCROSSSECTIONINTERNALWINDOW_EDITTABLECONTROLLER_H
