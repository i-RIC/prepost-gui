#include "post2dwindowgeodatadataitem.h"

#include <guicore/pre/geodata/geodata.h>
#include <guicore/pre/geodata/geodataproxy.h>

#include <QXmlStreamWriter>

Post2dWindowGeoDataDataItem::Post2dWindowGeoDataDataItem(GraphicsWindowDataItem* parent)
	: GeoDataProxyDataItem(parent)
{
	m_geoDataProxy = nullptr;
	m_isReorderable = false;
	m_isDeletable = false;
}

Post2dWindowGeoDataDataItem::~Post2dWindowGeoDataDataItem()
{
	if (m_geoDataProxy) {delete m_geoDataProxy;}
	m_geoDataProxy = nullptr;
}

void Post2dWindowGeoDataDataItem::setGeoDataProxy(GeoDataProxy* proxy)
{
	// set geodata
	m_geoDataProxy = proxy;
	m_geoDataProxy->setParent(this);
	// setup vtk actors.
	m_geoDataProxy->setupActors();
	m_geoDataProxy->setupMenu();
	updateZDepthRangeItemCount();
}

void Post2dWindowGeoDataDataItem::updateCaption(const QString& newcaption)
{
	m_standardItem->setText(newcaption);
}

void Post2dWindowGeoDataDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_geoDataProxy->loadFromProjectMainFile(node);
}

void Post2dWindowGeoDataDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_geoDataProxy->geoData()->name());
	m_geoDataProxy->saveToProjectMainFile(writer);
}

QDialog* Post2dWindowGeoDataDataItem::propertyDialog(QWidget* parent)
{
	return m_geoDataProxy->propertyDialog(parent);
}

void Post2dWindowGeoDataDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	m_geoDataProxy->handlePropertyDialogAccepted(propDialog);
	renderGraphicsView();
}

void Post2dWindowGeoDataDataItem::updateZDepthRangeItemCount()
{
	m_geoDataProxy->updateZDepthRangeItemCount(m_zDepthRange);
}

void Post2dWindowGeoDataDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_geoDataProxy->assignActorZValues(range);
}
