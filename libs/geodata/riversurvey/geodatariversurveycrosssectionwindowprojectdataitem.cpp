#include "geodatariversurvey.h"
#include "geodatariversurveycrosssectionwindow.h"
#include "geodatariversurveycrosssectionwindowprojectdataitem.h"

#include <guicore/pre/base/preprocessorgeodatagroupdataitemi.h>

GeoDataRiverSurveyCrosssectionWindowProjectDataItem::GeoDataRiverSurveyCrosssectionWindowProjectDataItem(PreProcessorGeoDataGroupDataItemI* gitem, QWidget* parentWindow) :
	ProjectDataItem {gitem}
{
	GeoDataRiverSurveyCrosssectionWindow* w = new GeoDataRiverSurveyCrosssectionWindow(gitem, this, parentWindow);
	m_window = w;
}


GeoDataRiverSurveyCrosssectionWindowProjectDataItem::~GeoDataRiverSurveyCrosssectionWindowProjectDataItem()
{
	if (m_window != nullptr) {
		GeoDataRiverSurveyCrosssectionWindow* w = m_window;
		m_window = nullptr;
		delete w->parent();
	}
}

void GeoDataRiverSurveyCrosssectionWindowProjectDataItem::informWindowClose()
{
	if (m_window != nullptr) {
		PreProcessorGeoDataGroupDataItemI* gItem = m_window->groupDataItem();
		m_window = nullptr;
		gItem->requestCrosssectionWindowDelete(this);
	}
}

void GeoDataRiverSurveyCrosssectionWindowProjectDataItem::requestWindowClose()
{
	m_window->groupDataItem()->requestCrosssectionWindowDelete(this);
}

void GeoDataRiverSurveyCrosssectionWindowProjectDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void GeoDataRiverSurveyCrosssectionWindowProjectDataItem::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{}
