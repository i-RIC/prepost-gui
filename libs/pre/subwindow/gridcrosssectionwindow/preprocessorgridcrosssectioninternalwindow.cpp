#include "preprocessorgridcrosssectioninternalwindow.h"
#include "preprocessorgridcrosssectionwindow.h"
#include "../../datamodel/preprocessorgriddataitem.h"
#include "../../datamodel/preprocessorgridtypedataitem.h"

#include <guicore/grid/v4structured2dgrid.h>
#include <guicore/pre/grid/v4inputgrid.h>

PreProcessorGridCrosssectionInternalWindow::PreProcessorGridCrosssectionInternalWindow(PreProcessorGridDataItem* item, QWidget *parent) :
	AbstractCrosssectionWindow {parent},
	m_item {item}
{}

PreProcessorGridCrosssectionInternalWindow::~PreProcessorGridCrosssectionInternalWindow()
{}

QMdiSubWindow* PreProcessorGridCrosssectionInternalWindow::mdiSubWindow() const
{
	auto w = dynamic_cast<PreProcessorGridCrosssectionWindow*> (parentWidget());
	return dynamic_cast<QMdiSubWindow*> (w->parentWidget());
}

v4Structured2dGrid* PreProcessorGridCrosssectionInternalWindow::grid()
{
	return dynamic_cast<v4Structured2dGrid*> (m_item->grid()->grid());
}

ColorMapSettingContainerI* PreProcessorGridCrosssectionInternalWindow::preColorMapSetting(const std::string& name) const
{
	return m_item->gridTypeDataItem()->colorMapSetting(name);
}
