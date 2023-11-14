#include "post3dwindowparticlesbasetopdataitem.h"
#include "post3dwindowparticlesbasevectorgroupdataitem.h"
#include "post3dwindowparticlesbasevectorgrouptopdataitem.h"
#include "post3dwindowzonedataitem.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/grid/v4particles3d.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>

#include <unordered_map>

Post3dWindowParticlesBaseVectorGroupTopDataItem::Post3dWindowParticlesBaseVectorGroupTopDataItem(Post3dWindowDataItem* parent) :
	Post3dWindowDataItem(tr("Arrows"), QIcon(":/libs/guibase/images/iconFolder.svg"), parent)
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);

	for (auto name : vtkDataSetAttributesTool::getArrayNamesWithMultipleComponents(topDataItem()->particleData()->vtkConcreteData()->concreteData()->GetPointData())) {
		auto item = new Post3dWindowParticlesBaseVectorGroupDataItem(name, this);
		m_childItems.push_back(item);
	}
}

Post3dWindowParticlesBaseVectorGroupTopDataItem::~Post3dWindowParticlesBaseVectorGroupTopDataItem()
{
	clearChildItems();
}

std::unordered_set<ColorMapSettingContainerI*> Post3dWindowParticlesBaseVectorGroupTopDataItem::activeColorMapSettingsWithVisibleLegend() const
{
	std::unordered_set<ColorMapSettingContainerI*> ret;

	for (auto c : m_childItems) {
		auto item = dynamic_cast<Post3dWindowParticlesBaseVectorGroupDataItem*> (c);
		auto cm = item->activeColorMapSettingWithVisibleLegend();
		if (cm != nullptr) {ret.insert(cm);}
	}

	return ret;
}

void Post3dWindowParticlesBaseVectorGroupTopDataItem::update()
{
	for (auto c : m_childItems) {
		auto item = dynamic_cast<Post3dWindowParticlesBaseVectorGroupDataItem*> (c);
		item->update();
	}
}

void Post3dWindowParticlesBaseVectorGroupTopDataItem::handleStandardItemChange()
{
	GraphicsWindowDataItem::handleStandardItemChange();
	topDataItem()->updateColorMapLegendsVisibility();
}

void Post3dWindowParticlesBaseVectorGroupTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	std::unordered_map<std::string, Post3dWindowParticlesBaseVectorGroupDataItem*> itemMap;
	for (auto c : m_childItems) {
		auto item = dynamic_cast<Post3dWindowParticlesBaseVectorGroupDataItem*> (c);
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

void Post3dWindowParticlesBaseVectorGroupTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto c : m_childItems) {
		auto item = dynamic_cast<Post3dWindowParticlesBaseVectorGroupDataItem*> (c);
		writer.writeStartElement("VectorGroup");
		writer.writeAttribute("target", item->target().c_str());
		item->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

Post3dWindowParticlesBaseTopDataItem* Post3dWindowParticlesBaseVectorGroupTopDataItem::topDataItem() const
{
	return dynamic_cast<Post3dWindowParticlesBaseTopDataItem*>(parent());
}

Post3dWindowZoneDataItem* Post3dWindowParticlesBaseVectorGroupTopDataItem::zoneDataItem() const
{
	return topDataItem()->zoneDataItem();
}

vtkPolyData* Post3dWindowParticlesBaseVectorGroupTopDataItem::particleData() const
{
	auto pd = topDataItem()->particleData();
	if (pd == nullptr) {return nullptr;}

	return pd->vtkConcreteData()->concreteData();
}
