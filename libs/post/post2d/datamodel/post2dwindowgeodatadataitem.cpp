#include "post2dwindowgeodatadataitem.h"
#include "post2dwindowgeodatagroupdataitem.h"
#include "post2dwindowgridtypedataitem.h"

#include <guicore/pre/geodata/geodata.h>
#include <guicore/pre/geodata/geodataproxy.h>

#include <QStandardItem>
#include <QXmlStreamWriter>

Post2dWindowGeoDataDataItem::Post2dWindowGeoDataDataItem(GraphicsWindowDataItem* parent) :
	GeoDataProxyDataItem {parent},
	m_geoDataProxy {nullptr}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
}

Post2dWindowGeoDataDataItem::~Post2dWindowGeoDataDataItem()
{
	delete m_geoDataProxy;
}

Post2dWindowGeoDataGroupDataItem* Post2dWindowGeoDataDataItem::groupDataItem() const
{
	return dynamic_cast<Post2dWindowGeoDataGroupDataItem*> (parent());
}

GeoDataProxy* Post2dWindowGeoDataDataItem::geoDataProxy() const
{
	return m_geoDataProxy;
}

void Post2dWindowGeoDataDataItem::setGeoDataProxy(GeoDataProxy* proxy)
{
	// set geodata
	m_geoDataProxy = proxy;
	m_geoDataProxy->setParent(this);
	// setup vtk actors.
	m_geoDataProxy->setupActors();
	m_geoDataProxy->setupMenu();

	connect(m_geoDataProxy, &GeoDataProxy::valueRangeChanged, this, &Post2dWindowGeoDataDataItem::informValueRangeChange);

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

bool Post2dWindowGeoDataDataItem::addToolBarButtons(QToolBar* toolBar)
{
	return groupDataItem()->addToolBarButtons(toolBar);
}

DelegatedColorMapSettingContainer* Post2dWindowGeoDataDataItem::colorMapSetting() const
{
	return groupDataItem()->colorMapSetting();
}

void Post2dWindowGeoDataDataItem::showPropertyDialog()
{
	m_geoDataProxy->showPropertyDialog();
}

QDialog* Post2dWindowGeoDataDataItem::propertyDialog(QWidget* parent)
{
	return m_geoDataProxy->propertyDialog(parent);
}

void Post2dWindowGeoDataDataItem::updateZDepthRangeItemCount()
{
	m_geoDataProxy->updateZDepthRangeItemCount(m_zDepthRange);
}

GraphicsWindowDataModel* Post2dWindowGeoDataDataItem::dataModel() const
{
	return GraphicsWindowDataItem::dataModel();
}

void Post2dWindowGeoDataDataItem::handleStandardItemChange()
{
	GraphicsWindowDataItem::handleStandardItemChange();

	auto gItem = groupDataItem();
	gItem->gridTypeDataItem()->updateColorBarVisibility(gItem->condition()->name());
}

void Post2dWindowGeoDataDataItem::informValueRangeChange()
{
	groupDataItem()->handleValueRangeChange();
}

bool Post2dWindowGeoDataDataItem::getValueRange(double* min, double* max)
{
	if (m_geoDataProxy == nullptr) {return false;}
	return m_geoDataProxy->geoData()->getValueRange(min, max);
}

void Post2dWindowGeoDataDataItem::applyColorMapSetting()
{
	if (m_geoDataProxy == nullptr) {return;}
	m_geoDataProxy->applyColorMapSetting();
}

void Post2dWindowGeoDataDataItem::assignActorZValues(const ZDepthRange& range)
{
	m_geoDataProxy->assignActorZValues(range);
}

void Post2dWindowGeoDataDataItem::doViewOperationEndedGlobal(VTKGraphicsView* v)
{
	m_geoDataProxy->viewOperationEndedGlobal(v);
}
