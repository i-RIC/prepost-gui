#include "preprocessorgridandgridcreatingconditiondataitem.h"
#include "preprocessorgridattributecelldataitem.h"
#include "preprocessorgridattributecellgroupdataitem.h"
#include "preprocessorgridattributenodedataitem.h"
#include "preprocessorgridattributenodegroupdataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgeodatacomplexgroupdataitem.h"
#include "preprocessorgeodatagroupdataitem.h"
#include "preprocessorgeodatatopdataitem.h"

#include <geodata/polygon/geodatapolygontrianglethread.h>
#include <guibase/vtkCustomScalarBarActor.h>
#include <guibase/graphicsmisc.h>
#include <guicore/datamodel/vtkgraphicsview.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorgraphicsviewinterface.h>
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/solverdef/solverdefinition.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridattributerealnode.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>

#include <QDomDocument>
#include <QDomElement>
#include <QDomNode>
#include <QIcon>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <vtkColorTransferFunction.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTextProperty.h>

#include <iriclib_errorcodes.h>

#include <map>

// namespace for local functions
namespace {

PreProcessorGeoDataGroupDataItem* setupReferenceInformation(
		std::vector<GraphicsWindowDataItem*>* children,
		std::map<std::string, PreProcessorGeoDataGroupDataItemInterface*>* nameMap,
		SolverDefinitionGridAttribute* refAtt,
		PreProcessorDataItem* parent)
{
	auto i = new PreProcessorGeoDataGroupDataItem(refAtt, parent);
	children->push_back(i);
	nameMap->insert({refAtt->name(), i});
	return i;
}

void setupChildrenInGroups(
		const std::vector<SolverDefinitionGridAttribute*>& stdAtts,
		const std::vector<SolverDefinitionGridComplexAttribute*>& clxAtts,
		std::vector <GraphicsWindowDataItem*>* children,
		std::map<std::string, PreProcessorGeoDataGroupDataItemInterface*>* nameMap,
		SolverDefinitionGridAttribute* refAtt,
		PreProcessorDataItem* parent)
{
	// node simple items
	for (auto att : stdAtts) {
		if (att->position() != SolverDefinitionGridAttribute::Node) {continue;}
		auto i = new PreProcessorGeoDataGroupDataItem(att, parent);
		children->push_back(i);
		nameMap->insert({att->name(), i});
	}
	// node complex items
	for (auto att : clxAtts) {
		if (att->position() != SolverDefinitionGridAttribute::Node) {continue;}
		auto i = new PreProcessorGeoDataComplexGroupDataItem(att, parent);
		children->push_back(i);
		nameMap->insert({att->name(), i});
	}
	// cell simple items
	for (auto att : stdAtts) {
		if (att->position() != SolverDefinitionGridAttribute::CellCenter) {continue;}
		auto i = new PreProcessorGeoDataGroupDataItem(att, parent);
		children->push_back(i);
		nameMap->insert({att->name(), i});
	}
	// cell complex items
	for (auto att : clxAtts) {
		if (att->position() != SolverDefinitionGridAttribute::CellCenter) {continue;}
		auto i = new PreProcessorGeoDataComplexGroupDataItem(att, parent);
		children->push_back(i);
		nameMap->insert({att->name(), i});
	}
	setupReferenceInformation(children, nameMap, refAtt, parent);
}

void setupChildrenInOrder(
		const std::vector<SolverDefinitionGridAttribute*>& stdAtts,
		const std::vector<SolverDefinitionGridComplexAttribute*>& clxAtts,
		std::vector<GraphicsWindowDataItem*>* children,
		std::map<std::string, PreProcessorGeoDataGroupDataItemInterface*>* nameMap,
		SolverDefinitionGridAttribute* refAtt,
		PreProcessorDataItem* parent)
{
	std::map<int, PreProcessorDataItem*> itemsInOrder;

	// simple items
	for (auto att : stdAtts){
		auto i = new PreProcessorGeoDataGroupDataItem(att, parent);
		children->push_back(i);
		nameMap->insert({att->name(), i});
		itemsInOrder.insert({att->order(), i});
	}
	// complex items
	for (auto att : clxAtts) {
		auto i = new PreProcessorGeoDataComplexGroupDataItem(att, parent);
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
		PreProcessorDataItem* item = pair.second;
		parent->standardItem()->appendRow(item->standardItem());
	}
}

void removeNonGroupedComplexAttributes(PreProcessorGeoDataTopDataItem* item)
{
	const auto& children = item->childItems();
	for (auto c : children) {
		auto complex = dynamic_cast<PreProcessorGeoDataComplexGroupDataItem*> (c);
		if (complex == nullptr) {continue;}
		auto att = dynamic_cast<SolverDefinitionGridComplexAttribute*>(complex->condition());
		if (att->isGrouped()) {continue;}

		item->standardItem()->takeRow(complex->standardItem()->row());
	}
}

} // namespace

PreProcessorGeoDataTopDataItem::PreProcessorGeoDataTopDataItem(PreProcessorDataItem* parent) :
	PreProcessorGeoDataTopDataItemInterface {tr("Geographic Data"), QIcon(":/libs/guibase/images/iconFolder.svg"), parent},
	m_referenceInformationAttribute {nullptr}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
	setSubPath("geographicdata");
	buildReferenceInformationAttribute();

	if (gridType()->isKeepOrder()) {
		setupChildrenInOrder(gridType()->gridAttributes(), gridType()->gridComplexAttributes(), &m_childItems, &m_itemNameMap, m_referenceInformationAttribute, this);
	} else {
		setupChildrenInGroups(gridType()->gridAttributes(), gridType()->gridComplexAttributes(), &m_childItems, &m_itemNameMap, m_referenceInformationAttribute, this);
	}
	removeNonGroupedComplexAttributes(this);

	setupActors();
}

PreProcessorGeoDataTopDataItem::~PreProcessorGeoDataTopDataItem()
{
	delete m_referenceInformationAttribute;
}

void PreProcessorGeoDataTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	auto elem = node.toElement();
	QDomNodeList children = node.childNodes();
	for (int i = 0; i < children.count(); ++i) {
		QDomElement child = children.at(i).toElement();
		std::string name = iRIC::toStr(child.attribute("name"));
		auto it = m_itemNameMap.find(name);
		if (it != m_itemNameMap.end()) {
			it->second->loadFromProjectMainFile(child);
		}
	}

	GeoDataPolygonTriangleThread::instance()->setLastJobDraw();

	updateActorSettings();
}

void PreProcessorGeoDataTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		writer.writeStartElement("GeoDataGroup");
		(*it)->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void PreProcessorGeoDataTopDataItem::assignActorZValues(const ZDepthRange& range)
{
	std::vector<GraphicsWindowDataItem*> items = m_childItems;
	GraphicsWindowDataItem* elevItem = 0;

	for (auto it = items.begin(); it != items.end(); ++it) {
		auto i = *it;
		auto gItem = dynamic_cast<PreProcessorGeoDataGroupDataItem*> (i);
		if (gItem->condition()->name() == "Elevation") {
			elevItem = i;
			items.erase(it);
			break;
		}
	}
	if (elevItem != 0) {
		items.push_back(elevItem);
	}
	GraphicsWindowDataItem::assignActorZValues(range, items);
}

void PreProcessorGeoDataTopDataItem::buildReferenceInformationAttribute()
{
	QDomDocument doc;
	auto itemElem = doc.createElement("Item");
	itemElem.setAttribute("name", "_referenceinformation");
	itemElem.setAttribute("caption", tr("Reference Information"));

	auto solverDef = projectData()->solverDefinition();
	m_referenceInformationAttribute = new SolverDefinitionGridAttributeRealNode(itemElem, solverDef, 10000);
	m_referenceInformationAttribute->setIsReferenceInformation(true);
}

SolverDefinitionGridType* PreProcessorGeoDataTopDataItem::gridType() const
{
	return dynamic_cast<PreProcessorGridTypeDataItemInterface*>(parent())->gridType();
}

const QList<PreProcessorGeoDataGroupDataItemInterface*> PreProcessorGeoDataTopDataItem::groupDataItems() const
{
	QList<PreProcessorGeoDataGroupDataItemInterface*> ret;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		PreProcessorGeoDataGroupDataItemInterface* item = dynamic_cast<PreProcessorGeoDataGroupDataItemInterface*>(*it);
		ret.append(item);
	}
	return ret;
}

PreProcessorGeoDataGroupDataItemInterface* PreProcessorGeoDataTopDataItem::groupDataItem(const std::string& name) const
{
	auto it = m_itemNameMap.find(name);
	if (it == m_itemNameMap.end()) {return nullptr;}
	return it->second;
}

void PreProcessorGeoDataTopDataItem::informValueRangeChange(const std::string& name)
{
	emit valueRangeChanged(name);
}

void PreProcessorGeoDataTopDataItem::informDataChange()
{
	emit dataChanged();
}

void PreProcessorGeoDataTopDataItem::setupActors()
{
	updateActorSettings();
}

void PreProcessorGeoDataTopDataItem::updateActorSettings()
{
	updateVisibility();
}

QStringList PreProcessorGeoDataTopDataItem::getGeoDatasNotMapped()
{
	QStringList ret;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		PreProcessorGeoDataGroupDataItem* item = dynamic_cast<PreProcessorGeoDataGroupDataItem*>(*it);
		ret.append(item->getGeoDatasNotMapped());
	}
	return ret;
}

int PreProcessorGeoDataTopDataItem::saveToCgnsFile()
{
	int ier = GraphicsWindowDataItem::saveToCgnsFile();
	if (ier != IRIC_NO_ERROR) {return ier;}

	for (auto child : m_childItems) {
		auto gItem = dynamic_cast<PreProcessorGeoDataGroupDataItem*>(child);
		int ier = gItem->saveComplexGroupsToCgnsFile();
		if (ier != IRIC_NO_ERROR) {return ier;}
	}
	return IRIC_NO_ERROR;
}

void PreProcessorGeoDataTopDataItem::setDimensionsToFirst()
{
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		PreProcessorGeoDataGroupDataItem* gItem =
			dynamic_cast<PreProcessorGeoDataGroupDataItem*>(*it);
		gItem->setDimensionsToFirst();
	}
}

void PreProcessorGeoDataTopDataItem::clearDimensionsIfNoDataExists()
{
	for (auto child : m_childItems) {
		auto gItem = dynamic_cast<PreProcessorGeoDataGroupDataItem*>(child);
		gItem->clearDimensionsIfNoDataExists();
	}
}
