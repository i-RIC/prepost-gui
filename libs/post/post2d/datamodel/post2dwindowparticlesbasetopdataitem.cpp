#include "../post2dwindowgraphicsview.h"
#include "post2dwindowcalculationresultdataitem.h"
#include "post2dwindowparticlesbasetopdataitem.h"
#include "post2dwindowparticlesbasescalargroupdataitem.h"
#include "post2dwindowparticlesbasevectorgrouptopdataitem.h"
#include "post2dwindowzonedataitem.h"
#include "private/post2dwindowparticlesbasetopdataitem_attributebrowsercontroller.h"
#include "private/post2dwindowparticlesbasetopdataitem_impl.h"

#include <guicore/base/propertybrowser.h>
#include <guicore/grid/v4particles2d.h>
#include <guicore/image/imagesettingcontainer.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/scalarstocolors/colormaplegendsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/xmlsupport.h>

Post2dWindowParticlesBaseTopDataItem::Impl::Impl(Post2dWindowParticlesBaseTopDataItem* item) :
	m_attributeBrowserController {new AttributeBrowserController {item}},
	m_scalarGroupDataItem {nullptr},
	m_vectorGroupDataItem {nullptr},
	m_showAttributeBrowserAction {new QAction(Post2dWindowParticlesBaseTopDataItem::tr("Show Attribute Browser"), item)}
{}

Post2dWindowParticlesBaseTopDataItem::Impl::~Impl()
{
	delete m_attributeBrowserController;
}

Post2dWindowParticlesBaseTopDataItem::Post2dWindowParticlesBaseTopDataItem(const QString& caption, Post2dWindowDataItem* p) :
	Post2dWindowDataItem {caption, QIcon(":/libs/guibase/images/iconFolder.svg"), p},
	impl {new Impl {this}}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	connect(impl->m_showAttributeBrowserAction, &QAction::triggered, this, &Post2dWindowParticlesBaseTopDataItem::showAttributeBrowser);
}

Post2dWindowParticlesBaseTopDataItem::~Post2dWindowParticlesBaseTopDataItem()
{}

void Post2dWindowParticlesBaseTopDataItem::setup()
{
	bool vectorExist = false;
	vtkPolyData* partD = particleData()->vtkConcreteData()->concreteData();
	auto pd = partD->GetPointData();

	int num = pd->GetNumberOfArrays();
	for (int i = 0; i < num; i++) {
		vtkAbstractArray* tmparray = pd->GetArray(i);
		if (tmparray == nullptr) {
			continue;
		}
		if (tmparray->GetNumberOfComponents() > 1) {
			// vector attribute.
			vectorExist = true;
		}
	}
	impl->m_scalarGroupDataItem = new Post2dWindowParticlesBaseScalarGroupDataItem(this);
	m_childItems.push_back(impl->m_scalarGroupDataItem);

	if (vectorExist) {
		impl->m_vectorGroupDataItem = new Post2dWindowParticlesBaseVectorGroupTopDataItem(this);
		m_childItems.push_back(impl->m_vectorGroupDataItem);
	}
}


Post2dWindowParticlesBaseScalarGroupDataItem* Post2dWindowParticlesBaseTopDataItem::scalarGroupDataItem() const
{
	return impl->m_scalarGroupDataItem;
}

Post2dWindowZoneDataItem* Post2dWindowParticlesBaseTopDataItem::zoneDataItem() const
{
	return resultDataItem()->zoneDataItem();
}

Post2dWindowParticlesBaseVectorGroupTopDataItem* Post2dWindowParticlesBaseTopDataItem::vectorGroupDataItem() const
{
	return impl->m_vectorGroupDataItem;
}

Post2dWindowAttributeBrowserController* Post2dWindowParticlesBaseTopDataItem::attributeBrowserController() const
{
	return impl->m_attributeBrowserController;
}

QAction* Post2dWindowParticlesBaseTopDataItem::showAttributeBrowserAction() const
{
	return impl->m_showAttributeBrowserAction;
}

void Post2dWindowParticlesBaseTopDataItem::updateZDepthRangeItemCount()
{
	m_zDepthRange.setItemCount(1);
}

void Post2dWindowParticlesBaseTopDataItem::assignActorZValues(const ZDepthRange& range)
{
	double center = (range.min() + range.max()) * 0.5;

	if (impl->m_scalarGroupDataItem != nullptr) {
		auto r2 = range;
		r2.setMin(center);
		impl->m_scalarGroupDataItem->assignActorZValues(r2);
	}

	if (impl->m_vectorGroupDataItem != nullptr) {
		auto r2 = range;
		r2.setMax(center);
		impl->m_vectorGroupDataItem->assignActorZValues(r2);
	}
}

void Post2dWindowParticlesBaseTopDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	attributeBrowserController()->initialize();
}

void Post2dWindowParticlesBaseTopDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	attributeBrowserController()->clear();
}

void Post2dWindowParticlesBaseTopDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	attributeBrowserController()->update(event->pos(), v);
}

void Post2dWindowParticlesBaseTopDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	attributeBrowserController()->fix(event->pos(), v);
}

void Post2dWindowParticlesBaseTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(impl->m_showAttributeBrowserAction);
}

void Post2dWindowParticlesBaseTopDataItem::update()
{
	if (impl->m_scalarGroupDataItem != nullptr) {
		impl->m_scalarGroupDataItem->update();
	}
	if (impl->m_vectorGroupDataItem != nullptr) {
		impl->m_vectorGroupDataItem->update();
	}
}

void Post2dWindowParticlesBaseTopDataItem::updateColorMapLegendsVisibility()
{
	m_actor2DCollection->RemoveAllItems();
	if (impl->m_scalarGroupDataItem == nullptr) {return;}

	for (const auto& pair: impl->m_scalarGroupDataItem->colorMapSettings()) {
		pair.second->legendSetting()->imgSetting()->actor()->VisibilityOff();
	}

	auto view = dataModel()->graphicsView();
	for (const auto& cms : activeColorMapsWithVisibleLegend()) {
		cms->legendSetting()->imgSetting()->apply(view);
		m_actor2DCollection->AddItem(cms->legendSetting()->imgSetting()->actor());
	}

	updateVisibilityWithoutRendering();
}

void Post2dWindowParticlesBaseTopDataItem::showAttributeBrowser()
{
	attributeBrowserController()->initialize();
	auto w = dynamic_cast<Post2dWindow*> (mainWindow());
	w->propertyBrowser()->show();
}

std::unordered_set<ColorMapSettingContainerI*> Post2dWindowParticlesBaseTopDataItem::activeColorMapsWithVisibleLegend() const
{
	std::unordered_set<ColorMapSettingContainerI*> ret;

	auto cm1 = impl->m_scalarGroupDataItem->activeColorMapSettingWithVisibleLegend();
	if (cm1 != nullptr) {ret.insert(cm1);}

	if (impl->m_vectorGroupDataItem != nullptr) {
		auto colormaps = impl->m_vectorGroupDataItem->activeColorMapSettingsWithVisibleLegend();
		for (auto cm : colormaps) {
			ret.insert(cm);
		}
	}

	return ret;
}

void Post2dWindowParticlesBaseTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	if (impl->m_scalarGroupDataItem != nullptr) {
		QDomNode scalarNode = iRIC::getChildNode(node, "ScalarGroup");
		if (! scalarNode.isNull()) {
			impl->m_scalarGroupDataItem->loadFromProjectMainFile(scalarNode);
		}
	}
	if (impl->m_vectorGroupDataItem != nullptr) {
		QDomNode vectorNode = iRIC::getChildNode(node, "VectorGroup");
		if (! vectorNode.isNull()) {
			impl->m_vectorGroupDataItem->loadFromProjectMainFile(vectorNode);
		}
	}
}

void Post2dWindowParticlesBaseTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	if (impl->m_scalarGroupDataItem != nullptr) {
		writer.writeStartElement("ScalarGroup");
		impl->m_scalarGroupDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	if (impl->m_vectorGroupDataItem != nullptr) {
		writer.writeStartElement("VectorGroup");
		impl->m_vectorGroupDataItem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

QDialog* Post2dWindowParticlesBaseTopDataItem::propertyDialog(QWidget* parent)
{
	return impl->m_scalarGroupDataItem->propertyDialog(parent);
}

void Post2dWindowParticlesBaseTopDataItem::handlePropertyDialogAccepted(QDialog* propDialog)
{
	impl->m_scalarGroupDataItem->handlePropertyDialogAccepted(propDialog);
}
