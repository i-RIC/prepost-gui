#include "post2dwindowgeodatagroupdataitem.h"
#include "post2dwindowgeodatatopdataitem.h"

#include <guibase/objectbrowserview.h>
#include <guibase/widget/itemselectingdialog.h>
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

SolverDefinitionGridAttribute* getAttributeWithName(const std::string& name, SolverDefinitionGridType* gType, SolverDefinitionGridAttribute* ref)
{
	auto a = gType->gridAttribute(name);
	if (a != nullptr) {return a;}

	auto ca = gType->gridComplexAttribute(name);
	if (ca != nullptr) {return ca;}

	if (ref->name() == name) {return ref;}

	return nullptr;
}

Post2dWindowGeoDataGroupDataItem* setupReferenceInformation(
		std::vector<GraphicsWindowDataItem*>* children,
		SolverDefinitionGridAttribute* refAtt,
		Post2dWindowDataItem* parent)
{
	auto i = new Post2dWindowGeoDataGroupDataItem(refAtt, parent);
	children->push_back(i);
	return i;
}

void addIfNotExist(
		SolverDefinitionGridAttribute* att,
		int* pos,
		std::vector<SolverDefinitionGridAttribute*>* addAtts,
		std::vector<int>* addPositions,
		const std::unordered_set<std::string> existingNames
) {
	if (existingNames.find(att->name()) != existingNames.end()) {
		++ *pos;
	} else {
		addAtts->push_back(att);
		addPositions->push_back(*pos);
	}
}

void setupAddItemsInGroups(
		const std::vector<SolverDefinitionGridAttribute*>& stdAtts,
		const std::vector<SolverDefinitionGridComplexAttribute*>& clxAtts,
		SolverDefinitionGridAttribute* refAtt,
		const std::unordered_set<std::string>& existingNames,
		std::vector<SolverDefinitionGridAttribute*>* addAtts,
		std::vector<int>* addPositions
)
{
	int pos = 0;
	// node simple items
	for (auto att : stdAtts) {
		if (att->position() != SolverDefinitionGridAttribute::Position::Node) {continue;}
		addIfNotExist(att, &pos, addAtts, addPositions, existingNames);
	}
	// node complex items
	for (auto att : clxAtts) {
		if (att->position() != SolverDefinitionGridAttribute::Position::Node) {continue;}
		addIfNotExist(att, &pos, addAtts, addPositions, existingNames);
	}
	// cell simple items
	for (auto att : stdAtts) {
		if (att->position() != SolverDefinitionGridAttribute::Position::CellCenter) {continue;}
		addIfNotExist(att, &pos, addAtts, addPositions, existingNames);
	}
	// cell complex items
	for (auto att : clxAtts) {
		if (att->position() != SolverDefinitionGridAttribute::Position::CellCenter) {continue;}
		addIfNotExist(att, &pos, addAtts, addPositions, existingNames);
	}

	// iedge simple items
	for (auto att : stdAtts) {
		if (att->position() != SolverDefinitionGridAttribute::Position::IFace) {continue;}
		addIfNotExist(att, &pos, addAtts, addPositions, existingNames);
	}
	// iedge complex items
	for (auto att : clxAtts) {
		if (att->position() != SolverDefinitionGridAttribute::Position::IFace) {continue;}
		addIfNotExist(att, &pos, addAtts, addPositions, existingNames);
	}

	// jedge simple items
	for (auto att : stdAtts) {
		if (att->position() != SolverDefinitionGridAttribute::Position::JFace) {continue;}
		addIfNotExist(att, &pos, addAtts, addPositions, existingNames);
	}
	// jedge complex items
	for (auto att : clxAtts) {
		if (att->position() != SolverDefinitionGridAttribute::Position::JFace) {continue;}
		addIfNotExist(att, &pos, addAtts, addPositions, existingNames);
	}

	addIfNotExist(refAtt, &pos, addAtts, addPositions, existingNames);
}

void setupAddItemsInOrder(
		const std::vector<SolverDefinitionGridAttribute*>& stdAtts,
		const std::vector<SolverDefinitionGridComplexAttribute*>& clxAtts,
		SolverDefinitionGridAttribute* refAtt,
		const std::unordered_set<std::string>& existingNames,
		std::vector<SolverDefinitionGridAttribute*>* addAtts,
		std::vector<int>* addPositions
)
{
	std::map<int, SolverDefinitionGridAttribute*> attsInOrder;

	// simple items
	for (auto att : stdAtts){
		attsInOrder.insert({att->order(), att});
	}
	// complex items
	for (auto att : clxAtts) {
		attsInOrder.insert({att->order(), att});
	}
	attsInOrder.insert({10000, refAtt});


	int pos = 0;
	for (const auto& pair : attsInOrder) {
		addIfNotExist(pair.second, &pos, addAtts, addPositions, existingNames);
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

	// auto gType = ditem->gridType();
	setupReferenceInformation(&m_childItems, m_referenceInformationAttribute, this);

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
	for (auto child : m_childItems) {
		auto item = dynamic_cast<Post2dWindowGeoDataGroupDataItem*> (child);
		if (item->condition()->name() == name) {return item;}
	}

	return nullptr;
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
	clearChildItems();

	QDomNodeList children = node.childNodes();
	auto gType = m_preGeoDataTopDataItem->gridType();

	for (int i = 0; i < children.count(); ++i) {
		QDomElement child = children.at(i).toElement();
		std::string name = iRIC::toStr(child.attribute("name"));

		auto att = getAttributeWithName(name, gType, m_referenceInformationAttribute);
		if (att == nullptr) {continue;}

		auto item = new Post2dWindowGeoDataGroupDataItem(att, this);
		m_childItems.push_back(item);
		item->loadFromProjectMainFile(child);
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

void Post2dWindowGeoDataTopDataItem::addCustomMenuItems(QMenu* menu)
{
	menu->addAction(dataModel()->objectBrowserView()->addAction());
}

QDialog* Post2dWindowGeoDataTopDataItem::addDialog(QWidget* parent)
{
	m_addAttributes.clear();
	m_addPositions.clear();
	std::unordered_set<std::string> existingNames;
	for (auto child : m_childItems) {
		auto item = dynamic_cast<Post2dWindowGeoDataGroupDataItem*> (child);
		existingNames.insert(item->condition()->name());
	}

	auto gType = m_preGeoDataTopDataItem->gridType();
	if (gType->isKeepOrder()) {
		setupAddItemsInOrder(gType->gridAttributes(), gType->gridComplexAttributes(), m_referenceInformationAttribute, existingNames, &m_addAttributes, &m_addPositions);
	} else {
		setupAddItemsInGroups(gType->gridAttributes(), gType->gridComplexAttributes(), m_referenceInformationAttribute, existingNames, &m_addAttributes, &m_addPositions);
	}
	if (m_addAttributes.size() == 0) {return nullptr;}

	std::vector<QString> captions;
	for (const auto att : m_addAttributes) {
		captions.push_back(att->caption());
	}

	auto dialog = new ItemSelectingDialog(parent);
	dialog->setWindowTitle(tr("Add geographic data group"));
	dialog->setMessage(tr("Select the geographic data group to add"));
	dialog->setItems(captions);

	return dialog;
}

void Post2dWindowGeoDataTopDataItem::handleAddDialogAccepted(QDialog* propDialog)
{
	auto dialog = dynamic_cast<ItemSelectingDialog*> (propDialog);
	auto selected = dialog->selectedIndex();
	auto att = m_addAttributes.at(selected);
	auto pos = m_addPositions.at(selected);

	auto i = new Post2dWindowGeoDataGroupDataItem(att, this);
	m_childItems.insert(m_childItems.begin() + pos, i);
	i->updateChildren();

	int rowC = standardItem()->rowCount();
	for (int i = 0; i < rowC; ++i) {
		standardItem()->takeRow(0);
	}

	for (auto item : m_childItems) {
		standardItem()->appendRow(item->standardItem());
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
