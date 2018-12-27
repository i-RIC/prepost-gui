#include "../misc/preprocessorscalarbarlegendboxsettingdialog.h"
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
#include <guicore/pre/base/preprocessorwindowinterface.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/scalarstocolors/colortransferfunctioncontainer.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
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

#include <iriclib.h>

#include <map>

// namespace for local functions
namespace {

void setupReferenceInformation(
		std::vector <GraphicsWindowDataItem*>* children,
		std::map<std::string, PreProcessorGeoDataGroupDataItemInterface*>* nameMap,
		SolverDefinitionGridAttribute* refAtt,
		PreProcessorDataItem* parent)
{
	auto i = new PreProcessorGeoDataGroupDataItem(refAtt, parent);
	children->push_back(i);
	nameMap->insert({refAtt->name(), i});
}

void setupChildrenInGroups(
		const QList<SolverDefinitionGridAttribute*>& stdAtts,
		const QList<SolverDefinitionGridComplexAttribute*>& clxAtts,
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
		const QList<SolverDefinitionGridAttribute*>& stdAtts,
		const QList<SolverDefinitionGridComplexAttribute*>& clxAtts,
		std::vector <GraphicsWindowDataItem*>* children,
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
	setupReferenceInformation(children, nameMap, refAtt, parent);

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
	PreProcessorGeoDataTopDataItemInterface {tr("Geographic Data"), QIcon(":/libs/guibase/images/iconFolder.png"), parent},
	m_condition {nullptr},
	m_visible {"visible", true},
	m_referenceInformationAttribute {nullptr}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
	setSubPath("geographicdata");
	buildReferenceInformationAttribute();

	m_titleTextSetting.setPrefix("title");
	m_labelTextSetting.setPrefix("label");

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
	m_scalarBarWidget->SetInteractor(nullptr);
	m_legendBoxWidget->SetInteractor(nullptr);

	delete m_referenceInformationAttribute;
}

void PreProcessorGeoDataTopDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	m_visible.load(node);
	m_titleTextSetting.load(node);
	m_labelTextSetting.load(node);
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
}

void PreProcessorGeoDataTopDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	m_visible.save(writer);
	m_titleTextSetting.save(writer);
	m_labelTextSetting.save(writer);
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

void PreProcessorGeoDataTopDataItem::setupScalarBar()
{
	PreProcessorScalarBarLegendBoxSettingDialog dialog(preProcessorWindow());
	if (m_condition != nullptr) {
		if (dynamic_cast<SolverDefinitionGridComplexAttribute*>(m_condition) != nullptr || m_condition->isOption()) {
			PreProcessorGeoDataGroupDataItem* gItem = dynamic_cast<PreProcessorGeoDataGroupDataItem*>(groupDataItem(m_condition->name()));
			ScalarBarSetting& setting = gItem->scalarBarSetting();
			setting.loadFromRepresentation(m_legendBoxWidget->GetLegendBoxRepresentation());
		} else {
			PreProcessorGeoDataGroupDataItem* gItem = dynamic_cast<PreProcessorGeoDataGroupDataItem*>(groupDataItem(m_condition->name()));
			ScalarBarSetting& setting = gItem->scalarBarSetting();
			setting.loadFromRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
		}
	}
	dialog.setupComboBox(this);
	dialog.setActor2DVisibility(m_visible);
	dialog.setCondition(m_condition);
	dialog.setTitleTextSetting(m_titleTextSetting);
	dialog.setLabelTextSetting(m_labelTextSetting);
	if (dialog.exec() == QDialog::Accepted) {
		m_visible = dialog.actor2DVisibility();
		m_condition = dialog.condition();
		m_titleTextSetting = dialog.titleTextSetting();
		m_labelTextSetting = dialog.labelTextSetting();
		updateActorSettings();
	}
}

void PreProcessorGeoDataTopDataItem::setupActors()
{
	vtkRenderWindowInteractor* iren = renderer()->GetRenderWindow()->GetInteractor();
	m_scalarBarWidget = vtkSmartPointer<vtkScalarBarWidget>::New();
	m_scalarBarWidget->SetScalarBarActor(vtkCustomScalarBarActor::New());
	m_scalarBarWidget->SetEnabled(0);
	m_scalarBarWidget->SetInteractor(iren);

	m_legendBoxWidget = vtkSmartPointer<vtkLegendBoxWidget>::New();
	m_legendBoxWidget->SetEnabled(0);
	m_legendBoxWidget->SetInteractor(iren);

	QList<PreProcessorGeoDataGroupDataItemInterface*> groups = groupDataItems();
	std::string attName;

	// for legend box
	for (auto it = groups.begin(); it != groups.end(); ++it) {
		if (!(*it)->condition()->isOption()) {continue;}
		attName = (*it)->condition()->name();
		break;
	}
	updateActorSettings();
}

void PreProcessorGeoDataTopDataItem::updateActorSettings()
{
	if (m_scalarBarWidget == nullptr) {return;}
	if (m_legendBoxWidget == nullptr) {return;}
	m_scalarBarWidget->SetEnabled(0);
	m_legendBoxWidget->SetEnabled(0);

	if (m_condition == nullptr) {return;}
	PreProcessorGeoDataGroupDataItem* rdgItem = dynamic_cast<PreProcessorGeoDataGroupDataItem*>(groupDataItem(m_condition->name()));
	if (rdgItem == nullptr) { return; }

	if (dynamic_cast<SolverDefinitionGridComplexAttribute*>(m_condition) != nullptr  || m_condition->isOption()) {
		// discrete
		if (! m_visible) {return;}
		m_legendBoxWidget->SetEnabled(1);

		PreProcessorGeoDataGroupDataItemInterface* gItem = groupDataItem(m_condition->name());
		if (gItem == nullptr) { return; }
		ScalarBarSetting& sbSetting = dynamic_cast<PreProcessorGeoDataGroupDataItem*>(gItem)->scalarBarSetting();
		sbSetting.saveToRepresentation(m_legendBoxWidget->GetLegendBoxRepresentation());
		m_labelTextSetting.applySetting(m_legendBoxWidget->GetLegendBoxActor()->GetEntryTextProperty());

		updateLegendBoxItems();
	} else {
		// continuous
		if (! m_visible) {return;}
		m_scalarBarWidget->SetEnabled(1);
		PreProcessorGeoDataGroupDataItemInterface* gItem = groupDataItem(m_condition->name());
		if (gItem == nullptr) { return; }
		ScalarBarSetting& sbSetting = dynamic_cast<PreProcessorGeoDataGroupDataItem*>(gItem)->scalarBarSetting();
		sbSetting.saveToRepresentation(m_scalarBarWidget->GetScalarBarRepresentation());
		PreProcessorGridTypeDataItem* gtItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent());
		LookupTableContainer* cont = dynamic_cast<LookupTableContainer*>(gtItem->scalarsToColors(m_condition->name()));
		if (cont == nullptr) { return; }
		vtkScalarBarActor* scalarBarActor = m_scalarBarWidget->GetScalarBarActor();
		scalarBarActor->SetLookupTable(cont->vtkObj());
		scalarBarActor->SetNumberOfLabels(sbSetting.numberOfLabels);
		scalarBarActor->SetTitle(iRIC::toStr(rdgItem->scalarBarTitle()).c_str());
		scalarBarActor->SetLabelFormat(iRIC::toStr(sbSetting.labelFormat).c_str());
		m_titleTextSetting.applySetting(scalarBarActor->GetTitleTextProperty());
		m_labelTextSetting.applySetting(scalarBarActor->GetLabelTextProperty());
	}
}

void PreProcessorGeoDataTopDataItem::updateLegendBoxItems()
{
	if (m_condition == nullptr) {return;}
	PreProcessorGridTypeDataItem* gtItem = dynamic_cast<PreProcessorGridTypeDataItem*>(parent());
	ColorTransferFunctionContainer* ctfCont = dynamic_cast<ColorTransferFunctionContainer*>(gtItem->scalarsToColors(m_condition->name()));
	vtkLegendBoxActor* lActor = m_legendBoxWidget->GetLegendBoxActor();
	lActor->SetNumberOfEntries(ctfCont->englishEnumerations().size());
	vtkColorTransferFunction* ctf = dynamic_cast<vtkColorTransferFunction*>(ctfCont->vtkObj());
	int index = 0;
	double color[3] = {0, 0, 0};
	// set entries
	for (auto mapIt = ctfCont->englishEnumerations().begin(); mapIt != ctfCont->englishEnumerations().end(); mapIt++) {
		vtkSphereSource* sphere = vtkSphereSource::New();
		double num = mapIt.key();
		QString label = mapIt.value();
		ctf->GetColor(num, color);
		sphere->Update();
		lActor->SetEntry(index++, sphere->GetOutput(), iRIC::toStr(label).c_str(), color);
		sphere->Delete();
	}
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

void PreProcessorGeoDataTopDataItem::informSelection(VTKGraphicsView* /*v*/)
{
	m_scalarBarWidget->SetRepositionable(1);
	m_legendBoxWidget->SetRepositionable(1);
}

void PreProcessorGeoDataTopDataItem::informDeselection(VTKGraphicsView* /*v*/)
{
	m_scalarBarWidget->SetRepositionable(0);
	m_legendBoxWidget->SetRepositionable(0);
}

void PreProcessorGeoDataTopDataItem::keyPressEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	v->standardKeyPressEvent(event);
}

void PreProcessorGeoDataTopDataItem::keyReleaseEvent(QKeyEvent* event, VTKGraphicsView* v)
{
	v->standardKeyReleaseEvent(event);
}

void PreProcessorGeoDataTopDataItem::mouseDoubleClickEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseDoubleClickEvent(event);
}

void PreProcessorGeoDataTopDataItem::mouseMoveEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseMoveEvent(event);
}

void PreProcessorGeoDataTopDataItem::mousePressEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMousePressEvent(event);
}

void PreProcessorGeoDataTopDataItem::mouseReleaseEvent(QMouseEvent* event, VTKGraphicsView* v)
{
	v->standardMouseReleaseEvent(event);
}

void PreProcessorGeoDataTopDataItem::saveToCgnsFile(const int fn)
{
	cg_iRIC_GotoRawDataTop(fn);
	GraphicsWindowDataItem::saveToCgnsFile(fn);

	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		PreProcessorGeoDataGroupDataItem* gItem =
			dynamic_cast<PreProcessorGeoDataGroupDataItem*>(*it);
		gItem->saveComplexGroupsToCgnsFile(fn);
	}
}

void PreProcessorGeoDataTopDataItem::setDimensionsToFirst()
{
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		PreProcessorGeoDataGroupDataItem* gItem =
			dynamic_cast<PreProcessorGeoDataGroupDataItem*>(*it);
		gItem->setDimensionsToFirst();
	}
}
