#include "../../datamodel/rawdataproxydataitem.h"
#include "rawdata.h"
#include "rawdataproxy.h"

QMainWindow* RawDataProxy::mainWindow() const
{
	RawDataProxyDataItem* item = dynamic_cast<RawDataProxyDataItem*>(parent());
	return item->mainWindow();
}

void RawDataProxy::renderGraphicsView()
{
	RawDataProxyDataItem* item = dynamic_cast<RawDataProxyDataItem*>(parent());
	item->renderGraphicsView();
}

vtkActorCollection* RawDataProxy::actorCollection() const
{
	RawDataProxyDataItem* item = dynamic_cast<RawDataProxyDataItem*>(parent());
	return item->m_actorCollection;
}

vtkActor2DCollection* RawDataProxy::actor2DCollection() const
{
	RawDataProxyDataItem* item = dynamic_cast<RawDataProxyDataItem*>(parent());
	return item->m_actor2DCollection;
}

void RawDataProxy::setupDataItem()
{
	RawDataProxyDataItem* item = dynamic_cast<RawDataProxyDataItem*>(parent());
	QString fname = m_rawData->name();
	fname.append(".dat");
	item->setFilename(fname);
	item->standardItem()->setText(m_rawData->caption());
	item->standardItem()->setEditable(false);
	item->standardItem()->setCheckable(true);
	item->standardItem()->setCheckState(Qt::Checked);

	item->m_standardItemCopy = item->m_standardItem->clone();
}

void RawDataProxy::updateVisibility()
{
	RawDataProxyDataItem* item = dynamic_cast<RawDataProxyDataItem*>(parent());
	item->updateVisibility();
}

void RawDataProxy::updateVisibilityWithoutRendering()
{
	RawDataProxyDataItem* item = dynamic_cast<RawDataProxyDataItem*>(parent());
	item->updateVisibilityWithoutRendering();
}

bool RawDataProxy::isVisible()
{
	RawDataProxyDataItem* item = dynamic_cast<RawDataProxyDataItem*>(parent());
	return item->isAncientChecked() && item->standardItem()->checkState() == Qt::Checked;
}

vtkRenderer* RawDataProxy::renderer()
{
	RawDataProxyDataItem* item = dynamic_cast<RawDataProxyDataItem*>(parent());
	return item->renderer();
}

void RawDataProxy::updateZDepthRangeItemCount(ZDepthRange& range)
{
	range.setItemCount(0);
}

void RawDataProxy::assignActorZValues(const ZDepthRange& /*range*/)
{

}
