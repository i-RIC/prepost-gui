#include "../../datamodel/preprocessorgriddataitem.h"
#include "../../datamodel/preprocessorgridattributenodedataitem.h"
#include "preprocessorgridcrosssectionwindow.h"
#include "preprocessorgridcrosssectionwindowprojectdataitem.h"

#include <guicore/pre/grid/grid.h>
#include <guicore/pre/grid/structured2dgrid.h>

PreProcessorGridCrosssectionWindowProjectDataItem::PreProcessorGridCrosssectionWindowProjectDataItem(PreProcessorGridAttributeNodeDataItem* item, const QString& cond, QWidget* parentWindow)
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
	if (m_window != nullptr) {
		PreProcessorGridCrosssectionWindow* w = m_window;
		m_window = nullptr;
		delete w->parent();
		m_item->unregisterCrosssectionWindow(this);
	}
}

void PreProcessorGridCrosssectionWindowProjectDataItem::informWindowClose()
{
	if (m_window != nullptr) {
		m_window = nullptr;
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
