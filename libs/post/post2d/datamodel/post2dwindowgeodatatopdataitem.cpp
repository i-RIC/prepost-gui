#include "post2dwindowgeodatagroupdataitem.h"
#include "post2dwindowgeodatatopdataitem.h"

#include <guicore/pre/base/preprocessorgeodatatopdataitemi.h>
#include <guicore/project/projectdata.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridattributereal.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <pre/datamodel/preprocessorgeodatatopdataitem.h>
#include <misc/stringtool.h>

#include <QDomNode>
#include <QXmlStreamWriter>

#include <algorithm>

namespace {

Post2dWindowGeoDataGroupDataItem* setupReferenceInformation(
		std::vector<GraphicsWindowDataItem*>* children,
		std::map<std::string, Post2dWindowGeoDataGroupDataItem*>* nameMap,
		SolverDefinitionGridAttribute* refAtt,
		Post2dWindowDataItem* parent)
{
	auto i = new Post2dWindowGeoDataGroupDataItem(refAtt, parent);
	children->push_back(i);
	nameMap->insert({refAtt->name(), i});
	return i;
}

void setupChildrenInGroups(
		const std::vector<SolverDefinitionGridAttribute*>& stdAtts,
		const std::vector<SolverDefinitionGridComplexAttribute*>& clxAtts,
		std::vector<GraphicsWindowDataItem*>* children,
		std::map<std::string, Post2dWindowGeoDataGroupDataItem*>* nameMap,
		SolverDefinitionGridAttribute* refAtt,
		Post2dWindowDataItem* parent)
{
	// node simple items
	for (auto att : stdAtts) {
		if (att->position() != SolverDefinitionGridAttribute::Position::Node) {continue;}
		auto i = new Post2dWindowGeoDataGroupDataItem(att, parent);
		children->push_back(i);
		nameMap->insert({att->name(), i});
	}
	// node complex items
	for (auto att : clxAtts) {
		if (att->position() != SolverDefinitionGridAttribute::Position::Node) {continue;}
		auto i = new Post2dWindowGeoDataGroupDataItem(att, parent);
		children->push_back(i);
		nameMap->insert({att->name(), i});
	}
	// cell simple items
	for (auto att : stdAtts) {
		if (att->position() != SolverDefinitionGridAttribute::Position::CellCenter) {continue;}
		auto i = new Post2dWindowGeoDataGroupDataItem(att, parent);
		children->push_back(i);
		nameMap->insert({att->name(), i});
	}
	// cell complex items
	for (auto att : clxAtts) {
		if (att->position() != SolverDefinitionGridAttribute::Position::CellCenter) {continue;}
		auto i = new Post2dWindowGeoDataGroupDataItem(att, parent);
		children->push_back(i);
		nameMap->insert({att->name(), i});
	}

	// iedge simple items
	for (auto att : stdAtts) {
		if (att->position() != SolverDefinitionGridAttribute::Position::IFace) {continue;}
		auto i = new Post2dWindowGeoDataGroupDataItem(att, parent);
		children->push_back(i);
		nameMap->insert({att->name(), i});
	}
	// iedge complex items
	for (auto att : clxAtts) {
		if (att->position() != SolverDefinitionGridAttribute::Position::IFace) {continue;}
		auto i = new Post2dWindowGeoDataGroupDataItem(att, parent);
		children->push_back(i);
		nameMap->insert({att->name(), i});
	}

	// jedge simple items
	for (auto att : stdAtts) {
		if (att->position() != SolverDefinitionGridAttribute::Position::JFace) {continue;}
		auto i = new Post2dWindowGeoDataGroupDataItem(att, parent);
		children->push_back(i);
		nameMap->insert({att->name(), i});
	}
	// jedge complex items
	for (auto att : clxAtts) {
		if (att->position() != SolverDefinitionGridAttribute::Position::JFace) {continue;}
		auto i = new Post2dWindowGeoDataGroupDataItem(att, parent);
		children->push_back(i);
		nameMap->insert({att->name(), i});
	}

	setupReferenceInformation(children, nameMap, refAtt, parent);
}

void setupChildrenInOrder(
		const std::vector<SolverDefinitionGridAttribute*>& stdAtts,
		const std::vector<SolverDefinitionGridComplexAttribute*>& clxAtts,
		std::vector<GraphicsWindowDataItem*>* children,
		std::map<std::string, Post2dWindowGeoDataGroupDataItem*>* nameMap,
		SolverDefinitionGridAttribute* refAtt,
		Post2dWindowDataItem* parent)
{
	std::map<int, Post2dWindowDataItem*> itemsInOrder;

	// simple items
	for (auto att : stdAtts){
		auto i = new Post2dWindowGeoDataGroupDataItem(att, parent);
		children->push_back(i);
		nameMap->insert({att->name(), i});
		itemsInOrder.insert({att->order(), i});
	}
	// complex items
	for (auto att : clxAtts) {
		auto i = new Post2dWindowGeoDataGroupDataItem(att, parent);
		children->push_back(i);
		nameMap->insert({att->name(), i});
		itemsInOrder.insert({att->order(), i});
	}
	auto ref = setupReferenceInformation(children, nameMap, refAtt, parent);
	itemsInOrder.insert({10000, ref}); // Reference Information should be the last

	int rowC = parent->standardItem()->rowCount();
	for (int i = 0; i < rowC; ++i) {
		parent->standardItem()->takeRow(0);
	}

	for (auto pair : itemsInOrder) {
		Post2dWindowDataItem* item = pair.second;
		parent->standardItem()->appendRow(item->standardItem());
	}
}

void removeNonGroupedComplexAttributes(Post2dWindowGeoDataTopDataItem* item)
{
	const auto& children = item->childItems();
	for (auto c : children) {
		auto complex = dynamic_cast<Post2dWindowGeoDataGroupDataItem*> (c);
		if (complex == nullptr) {continue;}
		auto att = dynamic_cast<SolverDefinitionGridComplexAttribute*>(complex->condition());
		if (att == nullptr) {continue;}
		if (att->isGrouped()) {continue;}

		item->standardItem()->takeRow(complex->standardItem()->row());
	}
}

} // namespace

Post2dWindowGeoDataTopDataItem::Post2dWindowGeoDataTopDataItem(PreProcessorGeoDataTopDataItemI* ditem, Post2dWindowDataItem* parent) :
	Post2dWindowDataItem {tr("Geographic Data"), QIcon(":/libs/guibase/images/iconFolder.svg"), parent},
	m_preGeoDataTopDataItem {ditem}
{
	setupStandardItem(NotChecked, NotReorderable, NotDeletable);
	buildReferenceInformationAttribute();

	auto gType = ditem->gridType();
	if (gType->isKeepOrder()) {
		setupChildrenInOrder(gType->gridAttributes(), gType->gridComplexAttributes(), &m_childItems, &m_itemNameMap, m_referenceInformationAttribute, this);
	} else {
		setupChildrenInGroups(gType->gridAttributes(), gType->gridComplexAttributes(), &m_childItems, &m_itemNameMap, m_referenceInformationAttribute, this);
	}
	removeNonGroupedComplexAttributes(this);

	updateChildren();
	connect(ditem, SIGNAL(dataChanged()), this, SLOT(updateChildren()));
}

PreProcessorGeoDataTopDataItemI* Post2dWindowGeoDataTopDataItem::preGeoDataTopDataItem() const
{
	return m_preGeoDataTopDataItem;
}

Post2dWindowGeoDataGroupDataItem* Post2dWindowGeoDataTopDataItem::groupDataItem(const std::string& name) const
{
	auto it = m_itemNameMap.find(name);
	if (it == m_itemNameMap.end()) {return nullptr;}
	return it->second;
}

void Post2dWindowGeoDataTopDataItem::updateChildren()
{
	for (int i = 0; i < m_childItems.size(); ++i) {
		auto gItem = dynamic_cast<Post2dWindowGeoDataGroupDataItem*>(m_childItems.at(i));
		gItem->updateChildren();
	}
	assignActorZValues(m_zDepthRange);
}

void Post2dWindowGeoDataTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement child = children.at(i).toElement();
		std::string name = iRIC::toStr(child.attribute("name"));
		auto item = groupDataItem(name);
		if (item != nullptr) {
			item->loadFromProjectMainFile(child);
		}
	}
}

void Post2dWindowGeoDataTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		writer.writeStartElement("GeoDataGroup");
		(*it)->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}


void Post2dWindowGeoDataTopDataItem::buildReferenceInformationAttribute()
{
	QDomDocument doc;
	auto itemElem = doc.createElement("Item");
	itemElem.setAttribute("name", "_referenceinformation");
	itemElem.setAttribute("caption", tr("Reference Information"));

	auto solverDef = projectData()->solverDefinition();
	m_referenceInformationAttribute = new SolverDefinitionGridAttributeReal(itemElem, solverDef, SolverDefinitionGridAttribute::Position::Node, false, 10000);
	m_referenceInformationAttribute->setIsReferenceInformation(true);
}
