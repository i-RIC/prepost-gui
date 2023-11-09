#include "preprocessorbackgroundimagesdataitem.h"
#include "preprocessorgridandgridcreatingconditiondataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridtypedataitem.h"
#include "preprocessorgeodatagroupdataitem.h"
#include "preprocessorgeodatatopdataitem.h"
#include "preprocessorhydraulicdatatopdataitem.h"
#include "preprocessorrootdataitem.h"
#include "private/preprocessorgridtypedataitem_applycolormapsettingandrendercommand.h"
#include "private/preprocessorgridtypedataitem_applycolormapsettingdialog.h"
#include "private/preprocessorgridtypedataitem_toolbarwidgetcontroller.h"

#include <guicore/pre/base/preprocessorgraphicsviewi.h>
#include <guicore/grid/v4grid.h>
#include <guicore/pre/grid/v4inputgrid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/scalarstocolors/colormapsettingcontaineri.h>
#include <guicore/scalarstocolors/colormapsettingtoolbarwidget.h>
#include <guicore/scalarstocolors/colormaplegendsettingcontaineri.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/valuechangert.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

PreProcessorGridTypeDataItem::PreProcessorGridTypeDataItem(SolverDefinitionGridType* type, GraphicsWindowDataItem* parent) :
	PreProcessorGridTypeDataItemI {type->caption(), QIcon(":/libs/guibase/images/iconFolder.svg"), parent},
	m_gridType {type},
	m_geoDataTop {nullptr}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
	setSubPath(type->name().c_str());

	setupColorMapSettingContainers();

	// add raw data node and grid data node.
	m_geoDataTop = new PreProcessorGeoDataTopDataItem(this);
	connect(m_geoDataTop, SIGNAL(valueRangeChanged(std::string)), this, SLOT(changeValueRange(std::string)));
	m_childItems.push_back(m_geoDataTop);

	m_hydraulicDataTop = new PreProcessorHydraulicDataTopDataItem(this);
	m_childItems.push_back(m_hydraulicDataTop);

	// add default grid, if this gridtype is not optional
	if (! type->isOptional()) {
		std::string zonename;
		if (type->isPrimary()) {
			// use the special name
			zonename = "iRICZone";
		} else {
			zonename = nextChildZonename();
		}
		auto cond = new PreProcessorGridAndGridCreatingConditionDataItem(zonename, nextChildCaption(), this);
		m_conditions.append(cond);
		m_childItems.push_back(cond);
	}

	// setup action items.
	m_addNewGridAction = new QAction(PreProcessorGridTypeDataItem::tr("Add New Grid"), this);
	connect(m_addNewGridAction, SIGNAL(triggered()), this, SLOT(addNewCondition()));
	updateNewGridActionStatus();
}

PreProcessorGridTypeDataItem::~PreProcessorGridTypeDataItem()
{
	for (const auto& pair : m_colorMapSettingContainers) {
		delete pair.second;
	}

	auto r = renderer();
	for (const auto& pair : m_colorMapLegendActors) {
		r->RemoveActor2D(pair.second);
		pair.second->Delete();
	}
}

const std::string& PreProcessorGridTypeDataItem::name() const
{
	return m_gridType->name();
}

SolverDefinitionGridType* PreProcessorGridTypeDataItem::gridType() const
{
	return m_gridType;
}

PreProcessorGeoDataTopDataItemI* PreProcessorGridTypeDataItem::geoDataTop() const
{
	return m_geoDataTop;
}

PreProcessorHydraulicDataTopDataItemI* PreProcessorGridTypeDataItem::hydraulicDataTop() const
{
	return m_hydraulicDataTop;
}

const QList<PreProcessorGridAndGridCreatingConditionDataItemI*>& PreProcessorGridTypeDataItem::conditions() const
{
	return m_conditions;
}

PreProcessorGridAndGridCreatingConditionDataItemI* PreProcessorGridTypeDataItem::condition(const std::string& zonename) const
{
	for (auto cond : m_conditions) {
		if (cond->zoneName() == zonename) {return cond;}
	}
	return nullptr;
}

bool PreProcessorGridTypeDataItem::isChildDeletable(const PreProcessorGridAndGridCreatingConditionDataItemI * /*child*/) const
{
	// if this gridtype is not optional and there is only one
	// condition, this item is not deletable.
	if (! m_gridType->isOptional() && m_conditions.count() == 1) {
		return false;
	}
	return true;
}

void PreProcessorGridTypeDataItem::addCustomMenuItems(QMenu* menu)
{
	if (m_gridType->multiple()) {
		menu->addAction(m_addNewGridAction);
	} else {
		if (m_conditions.count() == 0) {
			menu->addAction(m_addNewGridAction);
		}
	}
}

void PreProcessorGridTypeDataItem::addNewCondition()
{
	auto cond = new PreProcessorGridAndGridCreatingConditionDataItem(nextChildZonename(), nextChildCaption(), this);
	// In case there is only one grid type, add the grid creating condition item as root item.
	int row = standardItem()->row();
	if (row == - 1) {
		standardItem()->takeRow(cond->standardItem()->row());
		// find the background image row.
		auto rootItem = dynamic_cast<PreProcessorRootDataItem*>(parent());
		int imgRow = rootItem->backgroundImagesDataItem()->standardItem()->row();
		dataModel()->itemModel()->insertRow(imgRow, cond->standardItem());
	}
	m_conditions.append(cond);
	m_childItems.push_back(cond);
	updateNewGridActionStatus();
	updateItemMap();
	// this operation is not undo-able.
	iRICUndoStack::instance().clear();
}

QString PreProcessorGridTypeDataItem::nextChildCaption()
{
	bool ok = true;
	// first, try "Region".
	QString nomination(tr("Region"));
	for (auto cond : m_conditions) {
		ok = ok && (cond->caption() != nomination);
	}
	if (ok) {return nomination;}
	nomination = tr("Region%1");
	QString nom;
	ok = false;
	int i = 2;
	while (! ok) {
		nom = nomination.arg(i);
		ok = true;
		for (auto cond : m_conditions) {
			ok = ok && (cond->caption() != nom);
		}
		++i;
	}
	return nom;
}

std::string PreProcessorGridTypeDataItem::nextChildZonename()
{
	QString nametemplate(m_gridType->name().c_str());
	nametemplate.append("%1");
	std::string nom;
	bool ok = false;
	int i = 1;
	while (! ok) {
		nom = iRIC::toStr(nametemplate.arg(i));
		ok = true;
		for (auto cond : m_conditions) {
			ok = ok && (cond->zoneName() != nom);
		}
		++i;
	}
	return nom;
}

void PreProcessorGridTypeDataItem::unregisterChild(GraphicsWindowDataItem* child)
{
	PreProcessorDataItem::unregisterChild(child);
	for (auto it = m_conditions.begin(); it != m_conditions.end(); ++it) {
		if (*it == child) {
			m_conditions.erase(it);
			return;
		}
	}
	updateNewGridActionStatus();
}

bool PreProcessorGridTypeDataItem::isChildCaptionAvailable(const QString& caption)
{
	for (auto cond : m_conditions) {
		if (cond->caption() == caption) {
			return false;
		}
	}
	return true;
}

ColorMapSettingContainerI* PreProcessorGridTypeDataItem::colorMapSetting(const std::string& attName) const
{
	auto it = m_colorMapSettingContainers.find(attName);
	if (it == m_colorMapSettingContainers.end()) {return nullptr;}

	return it->second;
}

std::unordered_map<std::string, ColorMapSettingContainerI*> PreProcessorGridTypeDataItem::colorMapSettings() const
{
	return m_colorMapSettingContainers;
}

void PreProcessorGridTypeDataItem::updateColorBarVisibility(const std::string& attName)
{
	static bool updating = false;
	if (updating) {return;}

	ValueChangerT<bool> updatingChanger(&updating, true);

	auto cm = colorMapSetting(attName);
	if (cm == nullptr) {return;}
	auto actor = cm->legendSetting()->imgSetting()->actor();
	m_actor2DCollection->RemoveItem(actor);
	actor->VisibilityOff();

	bool visible = false;
	if (m_geoDataTop == nullptr) {return;}

	auto gItem = dynamic_cast<PreProcessorGeoDataGroupDataItem*> (m_geoDataTop->groupDataItem(attName));
	visible = visible || gItem->colorBarShouldBeVisible();

	for (auto cond : m_conditions) {
		auto gItem = dynamic_cast<PreProcessorGridDataItem*> (cond->gridDataItem());
		visible = visible || gItem->colorBarShouldBeVisible(attName);
	}
	visible = visible && cm->legendSetting()->getVisible();
	if (visible) {
		auto v = dataModel()->graphicsView();
		cm->legendSetting()->imgSetting()->apply(v);
		m_actor2DCollection->AddItem(actor);
	}

	updateVisibility();
}

QAction* PreProcessorGridTypeDataItem::addNewGridAction() const
{
	return m_addNewGridAction;
}

void PreProcessorGridTypeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	// load colormap data
	QDomNode cmNode = iRIC::getChildNode(node, "ColormapSettings");
	if (! cmNode.isNull()){
		for (int i = 0; i < cmNode.childNodes().count(); ++i) {
			QDomElement elem = cmNode.childNodes().at(i).toElement();
			std::string name = iRIC::toStr(elem.attribute("name"));
			auto it = m_colorMapSettingContainers.find(name);
			if (it != m_colorMapSettingContainers.end()) {
				it->second->load(elem);
			}
		}
	}

	// load raw data.
	QDomNode gdNode = iRIC::getChildNode(node, "GeoData");
	QDomNode rdNode = iRIC::getChildNode(node, "RawData");
	if (! gdNode.isNull()) {
		m_geoDataTop->loadFromProjectMainFile(gdNode);
	} else if (! rdNode.isNull()) {
		m_geoDataTop->loadFromProjectMainFile(rdNode);
	}
	// load hydraulic data.
	QDomNode hdNode = iRIC::getChildNode(node, "HydraulicData");
	if (! hdNode.isNull()) {
		m_hydraulicDataTop->loadFromProjectMainFile(hdNode);
	}

	// load region datas.
	QDomNode c = node.firstChild();
	while (! c.isNull()) {
		if (c.nodeName() == "Region") {
			// find whether a corresponding condition already exists.
			bool found = false;
			for (auto cond : m_conditions) {
				if (cond->zoneName() == iRIC::toStr(c.toElement().attribute("zoneName"))) {
					// found!
					found = true;
					cond->loadFromProjectMainFile(c);
				}
			}
			if (! found) {
				auto cond = new PreProcessorGridAndGridCreatingConditionDataItem(nextChildZonename(), nextChildCaption(), this);
				cond->loadFromProjectMainFile(c);
				int row = standardItem()->row();
				if (row == - 1) {
					standardItem()->takeRow(cond->standardItem()->row());
					// find the background image row.
					PreProcessorRootDataItem* rootItem = dynamic_cast<PreProcessorRootDataItem*>(parent());
					int imgRow = rootItem->backgroundImagesDataItem()->standardItem()->row();
					dataModel()->itemModel()->insertRow(imgRow, cond->standardItem());
				}
				m_conditions.append(cond);
				m_childItems.push_back(cond);
			}
		}
		c = c.nextSibling();
	}

	// Reset GeoData dimensions
	m_geoDataTop->setDimensionsToFirst();
}

void PreProcessorGridTypeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	// write name.
	writer.writeAttribute("name", name().c_str());

	// write colormap data.
	writer.writeStartElement("ColormapSettings");
	for (const auto& pair : m_colorMapSettingContainers) {
		writer.writeStartElement("ColormapSetting");
		writer.writeAttribute("name", pair.first.c_str());
		pair.second->save(writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();

	// write raw data.
	writer.writeStartElement("GeoData");
	m_geoDataTop->saveToProjectMainFile(writer);
	writer.writeEndElement();

	// write hydraulic data.
	writer.writeStartElement("HydraulicData");
	m_hydraulicDataTop->saveToProjectMainFile(writer);
	writer.writeEndElement();

	// write region datas.
	for (auto it = m_conditions.begin(); it != m_conditions.end(); ++it) {
		writer.writeStartElement("Region");
		(*it)->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void PreProcessorGridTypeDataItem::setupColorMapSettingContainers()
{
	auto atts = m_gridType->gridAttributes();
	for (auto att : atts) {
		setupColorMapSettingContainer(att);
	}

	auto complexAtts = m_gridType->gridComplexAttributes();
	for (auto att : complexAtts) {
		setupColorMapSettingContainer(att);
	}
}

void PreProcessorGridTypeDataItem::setupColorMapSettingContainer(SolverDefinitionGridAttribute* att)
{
	auto r = renderer();

	auto c = att->createColorMapSettingContainer();
	c->legendSetting()->setVisible(true);
	c->legendSetting()->setTitle(att->caption());
	m_colorMapSettingContainers.insert({att->name(), c});

	auto actor = vtkActor2D::New();
	r->AddActor2D(actor);
	c->legendSetting()->imgSetting()->setActor(actor);
	m_colorMapLegendActors.insert({att->name(), actor});
}

void PreProcessorGridTypeDataItem::changeValueRange(const std::string& name)
{
	auto it = m_colorMapSettingContainers.find(name);
	if (it == m_colorMapSettingContainers.end()) {return;}
	ColorMapSettingContainerI* colormap = it->second;

	// The value range of the specified grid related condition is changed.
	// Check the new min and max values.
	bool valueExist = false;
	double min = 0;
	double max = 0;

	// check raw data
	auto  i = m_geoDataTop->groupDataItem(name);
	if (i == nullptr) {return;}
	valueExist = i->getValueRange(&min, &max);

	// check grid values.
	for (auto cond : m_conditions) {
		v4InputGrid* g = cond->gridDataItem()->grid();
		if (g != nullptr) {
			auto att = g->attribute(name);
			if (att == nullptr) {continue;}
			double tmpmin, tmpmax;
			if (att->getValueRange(&tmpmin, &tmpmax)) {
				if (tmpmin < min || (! valueExist)) {min = tmpmin;}
				if (tmpmax > max || (! valueExist)) {max = tmpmax;}
				valueExist = true;
			}
		}
	}
	if (max - min < 1E-4) {
		// the width is too small.
		double mid = (min + max) * 0.5;
		double width = mid * 0.01;
		if (width < 1E-4) {
			width = 1E-4;
		}
		min = mid - width;
		max = mid + width;
	}

	// Now, new min, max are stored.
	colormap->setAutoValueRange(min, max);

	applyColorMapSetting(name);

	renderGraphicsView();
}

void PreProcessorGridTypeDataItem::applyColorMapSetting(const std::string& name)
{
	auto i = m_geoDataTop->groupDataItem(name);
	i->applyColorMapSetting();

	for (const auto c : m_conditions) {
		c->gridDataItem()->applyColorMapSetting(name);
	}
	updateColorBarVisibility(name);

	emit colorMapSettingChanged(name);
}

void PreProcessorGridTypeDataItem::assignActorZValues(const ZDepthRange& range)
{
	if (m_childItems.size() == 0) { return; }

	/// assign ZDepthRanges to child items.
	double max = range.max();
	double rangeWidth = range.width();
	double divNum = 0;
	divNum += m_childItems.size() - 1;
	for (auto it = m_childItems.begin(); it != m_childItems.end(); ++it) {
		int itemCount = ((*it)->zDepthRange().itemCount() - 1);
		if (itemCount >= 0) {
			divNum += itemCount;
		}
	}
	double divWidth = rangeWidth / divNum;
	// assign regions to GridAndGridCreatingConditionDataItem instances.
	for (auto cit = m_conditions.begin(); cit != m_conditions.end(); ++cit) {
		int itemCount = ((*cit)->zDepthRange().itemCount() - 1);
		int itemCount2 = 0;
		if (itemCount >= 0) {
			itemCount2 = itemCount;
		}
		double min = max - itemCount2 * divWidth;
		if (min < range.min()) {min = range.min();}
		ZDepthRange r = (*cit)->zDepthRange();
		r.setMin(min);
		r.setMax(max);
		(*cit)->setZDepthRange(r);
		max = min - divWidth;
	}
	// geodata gets the lowest region.
	ZDepthRange r = m_geoDataTop->zDepthRange();
	r.setMin(range.min());
	r.setMax(max);
	m_geoDataTop->setZDepthRange(r);
}

void PreProcessorGridTypeDataItem::updateNewGridActionStatus()
{
	bool enable = (m_gridType->multiple() || (m_conditions.count() == 0));
	m_addNewGridAction->setEnabled(enable);
}

bool PreProcessorGridTypeDataItem::isGridEdited() const
{
	for (auto cit = m_conditions.begin(); cit != m_conditions.end(); ++cit) {
		bool edited = (*cit)->isGridEdited();
		if (edited) {return true;}
	}
	return false;
}

void PreProcessorGridTypeDataItem::setGridEdited()
{
	for (auto cit = m_conditions.begin(); cit != m_conditions.end(); ++cit) {
		(*cit)->setGridEdited();
	}
}

ModifyCommandDialog* PreProcessorGridTypeDataItem::createApplyColorMapSettingDialog(const std::string& name, QWidget* parent)
{
	return new ApplyColorMapSettingDialog(name, parent, this);
}

QUndoCommand* PreProcessorGridTypeDataItem::createApplyColorMapSettingAndRenderCommand(const std::string& name, QUndoCommand* command, bool apply)
{
	return new ApplyColorMapSettingAndRenderCommand(command, name, apply, this);
}

void PreProcessorGridTypeDataItem::doHandleResize(QResizeEvent* event, VTKGraphicsView* v)
{
	for (const auto& pair : m_colorMapSettingContainers) {
		pair.second->legendSetting()->imgSetting()->controller()->handleResize(event, v);
	}
}

ColorMapSettingToolBarWidgetController* PreProcessorGridTypeDataItem::createToolBarWidgetController(const std::string& name, QWidget* parent)
{
	auto att = m_gridType->gridAttribute(name);
	if (att == nullptr) {
		att = m_gridType->gridComplexAttribute(name);
		if (att == nullptr) {return nullptr;}
	}

	auto widget = att->createColorMapSettingToolbarWidget(parent);
	widget->hide();
	widget->setSetting(m_colorMapSettingContainers.at(name));
	return new ToolBarWidgetController(name, widget, this);
}
