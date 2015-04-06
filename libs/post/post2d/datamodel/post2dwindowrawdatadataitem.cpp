#include "post2dwindowrawdatadataitem.h"

#include <guicore/pre/rawdata/rawdata.h>
#include <guicore/pre/rawdata/rawdataproxy.h>

Post2dWindowRawDataDataItem::Post2dWindowRawDataDataItem(GraphicsWindowDataItem* parent)
	: RawDataProxyDataItem(parent)
{
	m_rawDataProxy = 0;
	m_isReorderable = false;
	m_isDeletable = false;
}

Post2dWindowRawDataDataItem::~Post2dWindowRawDataDataItem()
{
	if (m_rawDataProxy){delete m_rawDataProxy;}
	m_rawDataProxy = 0;
}

void Post2dWindowRawDataDataItem::setRawDataProxy(RawDataProxy *proxy)
{
	// set rawdata
	m_rawDataProxy = proxy;
	m_rawDataProxy->setParent(this);
	// setup vtk actors.
	m_rawDataProxy->setupActors();
	m_rawDataProxy->setupMenu();
	updateZDepthRangeItemCount();
}

void Post2dWindowRawDataDataItem::updateCaption(const QString& newcaption)
{
	m_standardItem->setText(newcaption);
}

void Post2dWindowRawDataDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_rawDataProxy->loadFromProjectMainFile(node);
}

void Post2dWindowRawDataDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_rawDataProxy->rawData()->name());
	m_rawDataProxy->saveToProjectMainFile(writer);
}

QDialog* Post2dWindowRawDataDataItem::propertyDialog(QWidget* parent)
{
	return m_rawDataProxy->propertyDialog(parent);
}

void Post2dWindowRawDataDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	m_rawDataProxy->handlePropertyDialogAccepted(propDialog);
	renderGraphicsView();
}

void Post2dWindowRawDataDataItem::updateZDepthRangeItemCount()
{
	m_rawDataProxy->updateZDepthRangeItemCount(m_zDepthRange);
}

void Post2dWindowRawDataDataItem::assignActionZValues(const ZDepthRange &range)
{
	m_rawDataProxy->assignActionZValues(range);
}
