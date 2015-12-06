#include "../post2dbirdeyewindow.h"
#include "post2dbirdeyewindowgridtypedataitem.h"
#include "post2dbirdeyewindowzonedataitem.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QAction>
#include <QList>
#include <QMenu>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <vtkPointData.h>

Post2dBirdEyeWindowGridTypeDataItem::Post2dBirdEyeWindowGridTypeDataItem(SolverDefinitionGridType* type, GraphicsWindowDataItem* parent) :
	Post2dBirdEyeWindowDataItem {type->caption(), QIcon(":/libs/guibase/images/iconFolder.png"), parent},
	m_gridType {type},
	m_isZoneDataItemsSetup {false}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
	setSubPath(type->name().c_str());

	setupZoneDataItems();

	// add raw data node and grid data node.
	// setup ScalarsToColors instances
	QList<PostZoneDataContainer*> containers = postSolutionInfo()->zoneContainers2D();
	if (containers.size() == 0) {return;}

	vtkPointData* pd = containers.at(0)->data()->GetPointData();
	for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(pd)) {
		setupScalarsToColors(name);
	}
	updateLookupTableRanges();
}

Post2dBirdEyeWindowGridTypeDataItem::~Post2dBirdEyeWindowGridTypeDataItem()
{
	for (auto z_it = m_zoneDatas.begin(); z_it != m_zoneDatas.end(); ++z_it) {
		delete *z_it;
	}
}

const std::string& Post2dBirdEyeWindowGridTypeDataItem::name() const
{
	return m_gridType->name();
}

void Post2dBirdEyeWindowGridTypeDataItem::setupZoneDataItems()
{
	// first, clear the current zonedata.
	for (auto z_it = m_zoneDatas.begin(); z_it != m_zoneDatas.end(); ++z_it) {
		delete *z_it;
	}
	m_zoneDatas.clear();
	const QList<PostZoneDataContainer*>& zones = postSolutionInfo()->zoneContainers2D();
	int num = 0;
	int zoneNum = 0;
	for (auto it = zones.begin(); it != zones.end(); ++it) {
		const PostZoneDataContainer* cont = (*it);
		if (cont->gridType() == m_gridType) {
			Post2dBirdEyeWindowZoneDataItem* zdata = new Post2dBirdEyeWindowZoneDataItem(cont->zoneName(), num++, this);
			m_zoneDatas.append(zdata);
			m_zoneDataNameMap.insert(cont->zoneName(), zdata);
			m_childItems.append(zdata);
			++ zoneNum;
		}
	}
	if (m_lookupTables.count() == 0 && zones.size() != 0) {
		vtkPointData* pd = zones.at(0)->data()->GetPointData();
		for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(pd)) {
			setupScalarsToColors(name);
		}
		updateLookupTableRanges();
	}
	assignActorZValues(m_zDepthRange);
	m_isZoneDataItemsSetup = (zoneNum != 0);
}

void Post2dBirdEyeWindowGridTypeDataItem::update()
{
	if (! m_isZoneDataItemsSetup) {
		setupZoneDataItems();
	}
	// update LookupTable range.
	updateLookupTableRanges();

	// update child items.
	for (auto it = m_zoneDatas.begin(); it != m_zoneDatas.end(); ++it) {
		(*it)->update();
	}
}

void Post2dBirdEyeWindowGridTypeDataItem::updateLookupTableRanges()
{
	for (auto it = m_lookupTables.begin(); it != m_lookupTables.end(); ++it) {
		std::string name = it.key();
		ScalarsToColorsContainer* cont = it.value();
		bool first = true;
		double range[2], min, max;
		min = 0; max = 0;
		for (auto zit = m_zoneDatas.begin(); zit != m_zoneDatas.end(); ++zit) {
			Post2dBirdEyeWindowZoneDataItem* zitem = *zit;
			if (zitem->dataContainer() == nullptr || zitem->dataContainer()->data() == nullptr) {continue;}
			vtkDataArray* dArray = zitem->dataContainer()->data()->GetPointData()->GetArray(name.c_str());
			if (dArray != nullptr) {
				dArray->GetRange(range);
				if (first || range[0] < min) {min = range[0];}
				if (first || range[1] > max) {max = range[1];}
				first = false;
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
		cont->setValueRange(min, max);
	}
}

void Post2dBirdEyeWindowGridTypeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNode ltNode = iRIC::getChildNode(node, "LookupTables");
	if (! ltNode.isNull()) {
		QDomNodeList tables = ltNode.childNodes();
		for (int i = 0; i < tables.length(); ++i) {
			QDomNode ltNode = tables.at(i);
			std::string ltName = iRIC::toStr(ltNode.toElement().attribute("name"));
			LookupTableContainer* cont = m_lookupTables.value(ltName, nullptr);
			if (cont != nullptr) {
				cont->loadFromProjectMainFile(ltNode);
			}
		}
	}
	QDomNode zonesNode = iRIC::getChildNode(node, "Zones");
	if (! zonesNode.isNull()) {
		QDomNodeList zones = zonesNode.childNodes();
		for (int i = 0; i < zones.length(); ++i) {
			QDomNode zoneNode = zones.at(i);
			std::string zoneName = iRIC::toStr(zoneNode.toElement().attribute("name"));
			Post2dBirdEyeWindowZoneDataItem* zdi = m_zoneDataNameMap.value(zoneName, nullptr);
			if (zdi != nullptr) {
				zdi->loadFromProjectMainFile(zoneNode);
			}
		}
	}
}

void Post2dBirdEyeWindowGridTypeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_gridType->name().c_str());
	writer.writeStartElement("LookupTables");
	for (auto lit = m_lookupTables.begin(); lit != m_lookupTables.end(); ++lit) {
		writer.writeStartElement("LookupTable");
		writer.writeAttribute("name", lit.key().c_str());
		LookupTableContainer* cont = lit.value();
		cont->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();
	writer.writeStartElement("Zones");
	for (auto zit = m_zoneDatas.begin(); zit != m_zoneDatas.end(); ++zit) {
		Post2dBirdEyeWindowZoneDataItem* zitem = *zit;
		writer.writeStartElement("Zone");
		zitem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();
}

void Post2dBirdEyeWindowGridTypeDataItem::setupScalarsToColors(const std::string& name)
{
	LookupTableContainer* c = new LookupTableContainer(this);
	m_lookupTables.insert(name, c);
}
