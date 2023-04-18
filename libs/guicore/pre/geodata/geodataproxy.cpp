#include "../../datamodel/geodataproxydataitem.h"
#include "../../base/iricmainwindowinterface.h"
#include "../../post/post2d/base/post2dwindowgeodatadataiteminterface.h"
#include "../../post/post2d/base/post2dwindowgridtypedataiteminterface.h"
#include "geodata.h"
#include "geodataproxy.h"

#include <guicore/scalarstocolors/colormaplegendsettingcontaineri.h>
#include <guicore/scalarstocolors/delegatedcolormapsettingcontainer.h>

#include <QStandardItem>

GeoDataProxy::GeoDataProxy(GeoData* geodata) :
	ProjectDataItem(geodata),
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
	auto item = dynamic_cast<GeoDataProxyDataItem*>(parent());
	return item->mainWindow();
}

void GeoDataProxy::renderGraphicsView()
{
	auto item = dynamic_cast<GeoDataProxyDataItem*>(parent());
	item->renderGraphicsView();
}

vtkActorCollection* GeoDataProxy::actorCollection() const
{
	auto item = dynamic_cast<GeoDataProxyDataItem*>(parent());
	return item->m_actorCollection;
}

vtkActor2DCollection* GeoDataProxy::actor2DCollection() const
{
	auto item = dynamic_cast<GeoDataProxyDataItem*>(parent());
	return item->m_actor2DCollection;
}

void GeoDataProxy::setupDataItem()
{
	auto item = dynamic_cast<GeoDataProxyDataItem*>(parent());
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
	auto item = dynamic_cast<GeoDataProxyDataItem*>(parent());
	item->updateVisibility();
}

void GeoDataProxy::updateVisibilityWithoutRendering()
{
	auto item = dynamic_cast<GeoDataProxyDataItem*>(parent());
	item->updateVisibilityWithoutRendering();
}

bool GeoDataProxy::isVisible()
{
	auto item = dynamic_cast<GeoDataProxyDataItem*>(parent());
	return item->isAncientChecked() && item->standardItem()->checkState() == Qt::Checked;
}

GraphicsWindowDataModel* GeoDataProxy::dataModel() const
{
	return geoDataDataItem()->dataModel();
}

void GeoDataProxy::pushCommand(QUndoCommand* com)
{
	auto item = dynamic_cast<GeoDataProxyDataItem*>(parent());
	item->pushCommand(com);
}

void GeoDataProxy::pushRenderCommand(QUndoCommand* com)
{
	auto item = dynamic_cast<GeoDataProxyDataItem*>(parent());
	item->pushRenderCommand(com, item);
}

void GeoDataProxy::showPropertyDialogModal()
{
	QDialog* propDialog = propertyDialog(mainWindow());
	if (propDialog == nullptr) {return;}
	int result = propDialog->exec();
	if (result == QDialog::Accepted) {
		handlePropertyDialogAccepted(propDialog);
	}
	delete propDialog;
}

void GeoDataProxy::showPropertyDialogModeless()
{
	QDialog* propDialog = propertyDialog(mainWindow());
	if (propDialog == nullptr) {return;}
	propDialog->setAttribute(Qt::WA_DeleteOnClose);
	connect(propDialog, &QObject::destroyed, iricMainWindow(), &iRICMainWindowInterface::exitModelessDialogMode);

	iricMainWindow()->enterModelessDialogMode();

	propDialog->show();
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

void GeoDataProxy::showPropertyDialog()
{
	showPropertyDialogModal();
}

QDialog* GeoDataProxy::propertyDialog(QWidget*)
{
	return nullptr;
}

void GeoDataProxy::handlePropertyDialogAccepted(QDialog*)
{}

Post2dWindowGeoDataDataItemInterface* GeoDataProxy::geoDataDataItem() const
{
	return dynamic_cast<Post2dWindowGeoDataDataItemInterface*> (parent());
}

Post2dWindowGridTypeDataItemInterface* GeoDataProxy::gridTypeDataItem() const
{
	return dynamic_cast<Post2dWindowGridTypeDataItemInterface*>(parent()->parent()->parent()->parent());
}

void GeoDataProxy::applyColorMapSetting()
{
	updateActorSetting();
}

ColorMapSettingContainerI* GeoDataProxy::colorMapSettingContainer() const
{
	auto setting = geoDataDataItem()->colorMapSetting();
	if (setting == nullptr) {return nullptr;}

	if (setting->usePreSetting) {
		return setting->preSetting->setting();
	} else {
		return setting->customSetting;
	}
}

void GeoDataProxy::updateActorSetting()
{}
