#include "../../datamodel/geodataproxydataitem.h"
#include "geodata.h"
#include "geodataproxy.h"

#include <QStandardItem>

GeoDataProxy::GeoDataProxy(GeoData* geodata) :
	ProjectDataItem(nullptr),
	m_geoData {geodata}
{}

GeoDataProxy::~GeoDataProxy()
{}

void GeoDataProxy::setupActors()
{}

void GeoDataProxy::setupMenu()
{}

QMainWindow* GeoDataProxy::mainWindow() const
{
	GeoDataProxyDataItem* item = dynamic_cast<GeoDataProxyDataItem*>(parent());
	return item->mainWindow();
}

void GeoDataProxy::renderGraphicsView()
{
	GeoDataProxyDataItem* item = dynamic_cast<GeoDataProxyDataItem*>(parent());
	item->renderGraphicsView();
}

vtkActorCollection* GeoDataProxy::actorCollection() const
{
	GeoDataProxyDataItem* item = dynamic_cast<GeoDataProxyDataItem*>(parent());
	return item->m_actorCollection;
}

vtkActor2DCollection* GeoDataProxy::actor2DCollection() const
{
	GeoDataProxyDataItem* item = dynamic_cast<GeoDataProxyDataItem*>(parent());
	return item->m_actor2DCollection;
}

void GeoDataProxy::setupDataItem()
{
	GeoDataProxyDataItem* item = dynamic_cast<GeoDataProxyDataItem*>(parent());
	QString fname = m_geoData->name();
	fname.append(".dat");
	item->setFilename(fname);
	item->standardItem()->setText(m_geoData->caption());
	item->standardItem()->setEditable(false);
	item->standardItem()->setCheckable(true);
	item->standardItem()->setCheckState(Qt::Checked);

	item->m_standardItemCopy = item->m_standardItem->clone();
}

void GeoDataProxy::updateVisibility()
{
	GeoDataProxyDataItem* item = dynamic_cast<GeoDataProxyDataItem*>(parent());
	item->updateVisibility();
}

void GeoDataProxy::updateVisibilityWithoutRendering()
{
	GeoDataProxyDataItem* item = dynamic_cast<GeoDataProxyDataItem*>(parent());
	item->updateVisibilityWithoutRendering();
}

bool GeoDataProxy::isVisible()
{
	GeoDataProxyDataItem* item = dynamic_cast<GeoDataProxyDataItem*>(parent());
	return item->isAncientChecked() && item->standardItem()->checkState() == Qt::Checked;
}

void GeoDataProxy::pushCommand(QUndoCommand* com)
{
	m_geoData->pushCommand(com);
}

void GeoDataProxy::pushRenderCommand(QUndoCommand* com)
{
	m_geoData->pushRenderCommand(com);
}

vtkRenderer* GeoDataProxy::renderer()
{
	GeoDataProxyDataItem* item = dynamic_cast<GeoDataProxyDataItem*>(parent());
	return item->renderer();
}

void GeoDataProxy::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(0);
}

void GeoDataProxy::assignActorZValues(const ZDepthRange& /*range*/)
{}

GeoData* GeoDataProxy::geoData() const
{
	return m_geoData;
}

QDialog* GeoDataProxy::propertyDialog(QWidget*)
{
	return nullptr;
}

void GeoDataProxy::handlePropertyDialogAccepted(QDialog*)
{}

void GeoDataProxy::updateGraphics()
{}
