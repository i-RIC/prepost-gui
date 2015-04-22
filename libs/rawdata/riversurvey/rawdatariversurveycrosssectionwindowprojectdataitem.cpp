#include "rawdatariversurvey.h"
#include "rawdatariversurveycrosssectionwindow.h"
#include "rawdatariversurveycrosssectionwindowprojectdataitem.h"

#include <guicore/pre/base/preprocessorrawdatagroupdataiteminterface.h>

RawDataRiverSurveyCrosssectionWindowProjectDataItem::RawDataRiverSurveyCrosssectionWindowProjectDataItem(PreProcessorRawDataGroupDataItemInterface* gitem, QWidget* parentWindow)
	: ProjectDataItem(gitem)
{
	RawDataRiverSurveyCrosssectionWindow* w = new RawDataRiverSurveyCrosssectionWindow(gitem, this, parentWindow);
	m_window = w;
}


RawDataRiverSurveyCrosssectionWindowProjectDataItem::~RawDataRiverSurveyCrosssectionWindowProjectDataItem(){
	if (m_window != nullptr){
		RawDataRiverSurveyCrosssectionWindow* w = m_window;
		m_window = nullptr;
		delete w->parent();
	}
}

void RawDataRiverSurveyCrosssectionWindowProjectDataItem::informWindowClose()
{
	if (m_window != nullptr){
		PreProcessorRawDataGroupDataItemInterface* gItem = m_window->groupDataItem();
		m_window = nullptr;
		gItem->requestCrosssectionWindowDelete(this);
	}
}

void RawDataRiverSurveyCrosssectionWindowProjectDataItem::requestWindowClose()
{
	m_window->groupDataItem()->requestCrosssectionWindowDelete(this);
}


void RawDataRiverSurveyCrosssectionWindowProjectDataItem::doLoadFromProjectMainFile(const QDomNode& /*node*/)
{}

void RawDataRiverSurveyCrosssectionWindowProjectDataItem::doSaveToProjectMainFile(QXmlStreamWriter& /*writer*/)
{}
