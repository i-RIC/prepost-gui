#include "../post2dwindowattributebrowsercontroller.h"
#include "post2dwindowparticlesbasetopdataitem.h"
#include "post2dwindowparticlesbasevectorgroupdataitem.h"
#include "post2dwindowparticlesbasevectorgrouptopdataitem.h"
#include "post2dwindowzonedataitem.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/grid/v4particles2d.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>

#include <unordered_map>

Post2dWindowParticlesBaseVectorGroupTopDataItem::Post2dWindowParticlesBaseVectorGroupTopDataItem(Post2dWindowDataItem* parent) :
	Post2dWindowDataItem(tr("Arrows"), QIcon(":/libs/guibase/images/iconFolder.svg"), parent)
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	for (auto name : vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(topDataItem()->particleData()->vtkData()->data()->GetPointData())) {
		auto item = new Post2dWindowParticlesBaseVectorGroupDataItem(name, this);
		m_childItems.push_back(item);
	}
}

Post2dWindowParticlesBaseVectorGroupTopDataItem::~Post2dWindowParticlesBaseVectorGroupTopDataItem()
{
	clearChildItems();
}

std::unordered_set<ColorMapSettingContainerI*> Post2dWindowParticlesBaseVectorGroupTopDataItem::activeColorMapSettingsWithVisibleLegend() const
{
	std::unordered_set<ColorMapSettingContainerI*> ret;

	for (auto c : m_childItems) {
		auto item = dynamic_cast<Post2dWindowParticlesBaseVectorGroupDataItem*> (c);
		auto cm = item->activeColorMapSettingWithVisibleLegend();
		if (cm != nullptr) {ret.insert(cm);}
	}

	return ret;
}

void Post2dWindowParticlesBaseVectorGroupTopDataItem::update()
{
	for (auto c : m_childItems) {
		auto item = dynamic_cast<Post2dWindowParticlesBaseVectorGroupDataItem*> (c);
		item->update();
	}
}

void Post2dWindowParticlesBaseVectorGroupTopDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	topDataItem()->attributeBrowserController()->initialize();
}

void Post2dWindowParticlesBaseVectorGroupTopDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	topDataItem()->attributeBrowserController()->clear();
}

void Post2dWindowParticlesBaseVectorGroupTopDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	topDataItem()->attributeBrowserController()->update(event->pos(), v);
}

void Post2dWindowParticlesBaseVectorGroupTopDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	topDataItem()->attributeBrowserController()->fix(event->pos(), v);
}

void Post2dWindowParticlesBaseVectorGroupTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(topDataItem()->showAttributeBrowserAction());
}

void Post2dWindowParticlesBaseVectorGroupTopDataItem::handleStandardItemChange()
{
	GraphicsWindowDataItem::handleStandardItemChange();
	topDataItem()->updateColorMapLegendsVisibility();
}

void Post2dWindowParticlesBaseVectorGroupTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	std::unordered_map<std::string, Post2dWindowParticlesBaseVectorGroupDataItem*> itemMap;
	for (auto c : m_childItems) {
		auto item = dynamic_cast<Post2dWindowParticlesBaseVectorGroupDataItem*> (c);
		itemMap.insert({item->target(), item});
	}

	if (node.nodeName() == "VectorGroup") {
		// old style
		auto target = iRIC::toStr(node.toElement().attribute("attribute"));
		auto it = itemMap.find(target);
		if (it != itemMap.end()) {
			it->second->loadFromProjectMainFile(node);
		}
	} else if (node.nodeName() == "VectorGroupTop") {
		QDomNodeList children = node.childNodes();
		for (int i = 0; i < children.count(); ++i) {
			QDomElement childElem = children.at(i).toElement();
			if (childElem.nodeName() == "VectorGroup") {
				auto target = iRIC::toStr(childElem.attribute("target"));
				auto it = itemMap.find(target);
				if (it != itemMap.end()) {
					it->second->loadFromProjectMainFile(childElem);
				}
			}
		}
	}
}

void Post2dWindowParticlesBaseVectorGroupTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto c : m_childItems) {
		auto item = dynamic_cast<Post2dWindowParticlesBaseVectorGroupDataItem*> (c);
		writer.writeStartElement("VectorGroup");
		writer.writeAttribute("target", item->target().c_str());
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

Post2dWindowParticlesBaseTopDataItem* Post2dWindowParticlesBaseVectorGroupTopDataItem::topDataItem() const
{
	return dynamic_cast<Post2dWindowParticlesBaseTopDataItem*>(parent());
}

Post2dWindowZoneDataItem* Post2dWindowParticlesBaseVectorGroupTopDataItem::zoneDataItem() const
{
	return topDataItem()->zoneDataItem();
}

vtkPolyData* Post2dWindowParticlesBaseVectorGroupTopDataItem::particleData() const
{
	auto pd = topDataItem()->particleData();
	if (pd == nullptr) {return nullptr;}

	return pd->vtkConcreteData()->concreteData();
}
