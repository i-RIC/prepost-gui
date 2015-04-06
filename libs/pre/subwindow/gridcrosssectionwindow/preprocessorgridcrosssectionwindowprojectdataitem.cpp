#include "preprocessorgridcrosssectionwindowprojectdataitem.h"
#include "preprocessorgridcrosssectionwindow.h"
#include <guicore/pre/grid/structured2dgrid.h>
#include "../../datamodel/preprocessorgriddataitem.h"
#include "../../datamodel/preprocessorgridrelatedconditionnodedataitem.h"
#include <guicore/pre/grid/grid.h>

PreProcessorGridCrosssectionWindowProjectDataItem::PreProcessorGridCrosssectionWindowProjectDataItem(PreProcessorGridRelatedConditionNodeDataItem* item, const QString& cond, QWidget* parentWindow)
	: ProjectDataItem(0)
{
	m_item = item;
	PreProcessorGridDataItem* gItem = dynamic_cast<PreProcessorGridDataItem*>(item->parent()->parent());
	setParent(gItem->grid());
	Structured2DGrid* grid2d = dynamic_cast<Structured2DGrid*>(gItem->grid());
	PreProcessorGridCrosssectionWindow* w = new PreProcessorGridCrosssectionWindow(grid2d, cond, this, parentWindow);
	m_window = w;
}

PreProcessorGridCrosssectionWindowProjectDataItem::~PreProcessorGridCrosssectionWindowProjectDataItem()
{
	if (m_window != 0){
		PreProcessorGridCrosssectionWindow* w = m_window;
		m_window = 0;
		delete w->parent();
		m_item->unregisterCrosssectionWindow(this);
	}
}

void PreProcessorGridCrosssectionWindowProjectDataItem::informWindowClose()
{
	if (m_window != 0){
		m_window = 0;
		m_item->requestCrosssectionWindowDelete(this);
	}
}

void PreProcessorGridCrosssectionWindowProjectDataItem::requestWindowClose()
{
	m_item->requestCrosssectionWindowDelete(this);
}

void PreProcessorGridCrosssectionWindowProjectDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void PreProcessorGridCrosssectionWindowProjectDataItem::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{}
