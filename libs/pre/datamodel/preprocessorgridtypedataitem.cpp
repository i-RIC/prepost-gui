#include "preprocessorbackgroundimagesdataitem.h"
#include "preprocessorgridandgridcreatingconditiondataitem.h"
#include "preprocessorgriddataitem.h"
#include "preprocessorgridtypedataitem.h"
#include "preprocessorgeodatagroupdataitem.h"
#include "preprocessorgeodatatopdataitem.h"
#include "preprocessorrootdataitem.h"

#include <guicore/pre/base/preprocessorgriddataiteminterface.h>
#include <guicore/pre/base/preprocessorgriddataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatagroupdataiteminterface.h>
#include <guicore/pre/base/preprocessorgeodatatopdataiteminterface.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/scalarstocolors/scalarstocolorscontainer.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridcomplexattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/iricundostack.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QAction>
#include <QDomNode>
#include <QMenu>
#include <QStandardItem>
#include <QXmlStreamWriter>

PreProcessorGridTypeDataItem::PreProcessorGridTypeDataItem(SolverDefinitionGridType* type, GraphicsWindowDataItem* parent) :
	PreProcessorGridTypeDataItemInterface {type->caption(), QIcon(":/libs/guibase/images/iconFolder.png"), parent},
	m_gridType {type}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
	setSubPath(type->name().c_str());

	// setup ScalarsToColors instances
	setupScalarsToColors(type);

	// add raw data node and grid data node.
	m_geoDataTop = new PreProcessorGeoDataTopDataItem(this);
	connect(m_geoDataTop, SIGNAL(valueRangeChanged(std::string)), this, SLOT(changeValueRange(std::string)));
	m_childItems.push_back(m_geoDataTop);

	// add default grid, if this gridtype is not optional
	if (! type->isOptional()) {
		std::string zonename;
		if (type->isPrimary()) {
			// use the special name
			zonename = "iRICZone";
		} else {
			zonename = nextChildZonename();
		}
		PreProcessorGridAndGridCreatingConditionDataItem* cond = new PreProcessorGridAndGridCreatingConditionDataItem(zonename, nextChildCaption(), this);
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
	for (auto it = m_scalarsToColors.begin(); it != m_scalarsToColors.end(); ++it) {
		delete it.value();
	}
}

const std::string& PreProcessorGridTypeDataItem::name() const
{
	return m_gridType->name();
}

PreProcessorGridAndGridCreatingConditionDataItemInterface* PreProcessorGridTypeDataItem::condition(const std::string& zonename) const
{
	for (auto cond : m_conditions) {
		if (cond->zoneName() == zonename) {return cond;}
	}
	return nullptr;
}

bool PreProcessorGridTypeDataItem::isChildDeletable(const PreProcessorGridAndGridCreatingConditionDataItemInterface * /*child*/) const
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
	PreProcessorGridAndGridCreatingConditionDataItem* cond = new PreProcessorGridAndGridCreatingConditionDataItem(nextChildZonename(), nextChildCaption(), this);
	// In case there is only one grid type, add the grid creating condition item as root item.
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

void PreProcessorGridTypeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	// load colormap data
	QDomNode cmNode = iRIC::getChildNode(node, "ColorMaps");
	if (! cmNode.isNull()){
		for (int i = 0; i < cmNode.childNodes().count(); ++i) {
			QDomElement elem = cmNode.childNodes().at(i).toElement();
			std::string name = iRIC::toStr(elem.attribute("name"));
			ScalarsToColorsContainer* cont = m_scalarsToColors.value(name, nullptr);
			if (cont == nullptr) {continue;}

			cont->loadFromProjectMainFile(elem);
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
				PreProcessorGridAndGridCreatingConditionDataItem* cond = new PreProcessorGridAndGridCreatingConditionDataItem(nextChildZonename(), nextChildCaption(), this);
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
	writer.writeStartElement("ColorMaps");
	for (auto c_it = m_scalarsToColors.begin(); c_it != m_scalarsToColors.end(); ++c_it) {
		writer.writeStartElement("ColorMap");
		writer.writeAttribute("name", c_it.key().c_str());
		c_it.value()->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();

	// write raw data.
	writer.writeStartElement("GeoData");
	m_geoDataTop->saveToProjectMainFile(writer);
	writer.writeEndElement();

	// write region datas.
	for (auto it = m_conditions.begin(); it != m_conditions.end(); ++it) {
		writer.writeStartElement("Region");
		(*it)->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
}

void PreProcessorGridTypeDataItem::setupScalarsToColors(SolverDefinitionGridType* type)
{
	QList<SolverDefinitionGridAttribute*> conditions = type->gridAttributes();
	QList<SolverDefinitionGridComplexAttribute*> conditions2 = type->gridComplexAttributes();
	for (auto it = conditions.begin(); it != conditions.end(); ++it) {
		ScalarsToColorsContainer* c = (*it)->createScalarsToColorsContainer(nullptr);
		m_scalarsToColors.insert((*it)->name(), c);
	}
	for (auto it2 = conditions2.begin(); it2 != conditions2.end(); ++it2) {
		ScalarsToColorsContainer* c = (*it2)->createScalarsToColorsContainer(nullptr);
		m_scalarsToColors.insert((*it2)->name(), c);
	}
}

void PreProcessorGridTypeDataItem::changeValueRange(const std::string& name)
{
	auto stc = m_scalarsToColors.value(name, nullptr);
	if (stc == nullptr) {return;}

	// The value range of the specified grid related condition is changed.
	// Check the new min and max values.
	bool valueExist = false;
	double min = 0;
	double max = 0;
	// check raw data
	PreProcessorGeoDataGroupDataItemInterface* i = m_geoDataTop->groupDataItem(name);
	if (i == nullptr) {return;}
	valueExist = i->getValueRange(&min, &max);

	// check grid values.
	for (auto it = m_conditions.begin(); it != m_conditions.end(); ++it) {
		Grid* g = (*it)->gridDataItem()->grid();
		if (g != nullptr) {
			GridAttributeContainer* c = g->gridAttribute(name);
			if (c == nullptr) {continue;}
			double tmpmin, tmpmax;
			if (c->getValueRange(&tmpmin, &tmpmax)) {
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
	stc->setValueRange(min, max);
	renderGraphicsView();
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
