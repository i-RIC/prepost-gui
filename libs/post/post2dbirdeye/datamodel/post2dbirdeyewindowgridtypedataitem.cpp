#include "../post2dbirdeyewindow.h"
#include "post2dbirdeyewindowgridtypedataitem.h"
#include "post2dbirdeyewindowzonedataitem.h"

#include <guibase/vtkdatasetattributestool.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/pre/gridcond/base/gridattributecontainer.h>
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <guicore/scalarstocolors/scalarstocolorscontainerutil.h>
#include <guicore/solverdef/solverdefinitiongridattribute.h>
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

#include <QAction>
#include <QDomNode>
#include <QList>
#include <QMenu>
#include <QStandardItem>
#include <QXmlStreamWriter>

#include <vtkCellData.h>
#include <vtkPointData.h>

namespace {

PostZoneDataContainer* getContainerWithZoneType(const QList<PostZoneDataContainer*>& conts, SolverDefinitionGridType* gt)
{
	for (auto container : conts) {
		if (container->gridType() == gt) {
			return container;
		}
	}
	return nullptr;
}

} // namespace

Post2dBirdEyeWindowGridTypeDataItem::Post2dBirdEyeWindowGridTypeDataItem(SolverDefinitionGridType* type, GraphicsWindowDataItem* parent) :
	Post2dBirdEyeWindowDataItem {type->caption(), QIcon(":/libs/guibase/images/iconFolder.png"), parent},
	m_gridType {type},
	m_isZoneDataItemsSetup {false}
{
	setupStandardItem(Checked, NotReorderable, NotDeletable);
	setSubPath(type->name().c_str());

	setupZoneDataItems();
}

Post2dBirdEyeWindowGridTypeDataItem::~Post2dBirdEyeWindowGridTypeDataItem()
{
	for (auto z_it = m_zoneDatas.begin(); z_it != m_zoneDatas.end(); ++z_it) {
		delete *z_it;
	}
}

const QList<Post2dBirdEyeWindowZoneDataItem*>& Post2dBirdEyeWindowGridTypeDataItem::zoneDatas() const
{
	return m_zoneDatas;
}

const std::string& Post2dBirdEyeWindowGridTypeDataItem::name() const
{
	return m_gridType->name();
}

Post2dBirdEyeWindowZoneDataItem* Post2dBirdEyeWindowGridTypeDataItem::zoneData(const std::string& name) const
{
	return m_zoneDataNameMap.value(name);
}

SolverDefinitionGridType* Post2dBirdEyeWindowGridTypeDataItem::gridType() const
{
	return m_gridType;
}

LookupTableContainer* Post2dBirdEyeWindowGridTypeDataItem::nodeLookupTable(const std::string& attName)
{
	if (m_nodeLookupTables.find(attName) == m_nodeLookupTables.end()) {
		setupNodeScalarsToColors(attName);
	}
	return m_nodeLookupTables.value(attName, nullptr);
}

LookupTableContainer* Post2dBirdEyeWindowGridTypeDataItem::cellLookupTable(const std::string& attName)
{
	if (m_cellLookupTables.find(attName) == m_cellLookupTables.end()) {
		setupCellScalarsToColors(attName);
	}
	return m_cellLookupTables.value(attName, nullptr);
}

LookupTableContainer* Post2dBirdEyeWindowGridTypeDataItem::particleLookupTable(const std::string&)
{
	return nullptr;
}

void Post2dBirdEyeWindowGridTypeDataItem::setupZoneDataItems()
{
	// first, clear the current zonedata.
	for (auto z_it = m_zoneDatas.begin(); z_it != m_zoneDatas.end(); ++z_it) {
		delete *z_it;
	}
	m_zoneDatas.clear();
	const QList<PostZoneDataContainer*>& zones = postSolutionInfo()->zoneContainers2D();

	PostZoneDataContainer* zCont = getContainerWithZoneType(zones, m_gridType);
	if (zCont != nullptr) {
		if (m_nodeLookupTables.count() == 0 && zones.size() != 0) {
			vtkPointData* pd = zCont->data()->GetPointData();
			for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(pd)) {
				setupNodeScalarsToColors(name);
			}
		}
		if (m_cellLookupTables.count() == 0 && zones.size() != 0) {
			vtkCellData* cd = zCont->data()->GetCellData();
			for (std::string name : vtkDataSetAttributesTool::getArrayNamesWithOneComponent(cd)) {
				setupCellScalarsToColors(name);
			}
		}
	}

	int num = 0;
	int zoneNum = 0;
	for (auto it = zones.begin(); it != zones.end(); ++it) {
		const PostZoneDataContainer* cont = (*it);
		if (cont->gridType() == m_gridType) {
			Post2dBirdEyeWindowZoneDataItem* zdata = new Post2dBirdEyeWindowZoneDataItem(cont->zoneName(), num++, this);
			m_zoneDatas.append(zdata);
			m_zoneDataNameMap.insert(cont->zoneName(), zdata);
			m_childItems.push_back(zdata);
			++ zoneNum;
		}
	}

	if (zCont != nullptr) {
		if (zones.size() != 0) {
			// must call updateLookupTableRanges after m_zoneDatas is filled
			// by setupSclarsToColors
			updateLookupTableRanges();
		}
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
	for (auto it = m_nodeLookupTables.begin(); it != m_nodeLookupTables.end(); ++it) {
		std::string name = it.key();
		ScalarsToColorsContainer* cont = it.value();
		std::vector<vtkDataArray*> da;
		for (auto zit = m_zoneDatas.begin(); zit != m_zoneDatas.end(); ++zit) {
			Post2dBirdEyeWindowZoneDataItem* zitem = *zit;
			if (zitem->dataContainer() == nullptr || zitem->dataContainer()->data() == nullptr) {continue;}
			vtkDataArray* dArray = zitem->dataContainer()->data()->GetPointData()->GetArray(name.c_str());
			if (dArray == nullptr) {continue;}
			da.push_back(dArray);
		}
		ScalarsToColorsContainerUtil::setValueRange(cont, da);
	}
	for (auto it = m_cellLookupTables.begin(); it != m_cellLookupTables.end(); ++it) {
		std::string name = it.key();
		ScalarsToColorsContainer* cont = it.value();
		std::vector<vtkDataArray*> da;
		for (auto zit = m_zoneDatas.begin(); zit != m_zoneDatas.end(); ++zit) {
			Post2dBirdEyeWindowZoneDataItem* zitem = *zit;
			if (zitem->dataContainer() == nullptr || zitem->dataContainer()->data() == nullptr) { continue; }
			vtkDataArray* dArray = zitem->dataContainer()->data()->GetCellData()->GetArray(name.c_str());
			if (dArray == nullptr) { continue; }
			da.push_back(dArray);
		}
		ScalarsToColorsContainerUtil::setValueRange(cont, da);
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
			LookupTableContainer* cont = m_nodeLookupTables.value(ltName, nullptr);
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
	for (auto lit = m_nodeLookupTables.begin(); lit != m_nodeLookupTables.end(); ++lit) {
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

void Post2dBirdEyeWindowGridTypeDataItem::setupNodeScalarsToColors(const std::string& name)
{
	LookupTableContainer* c = new LookupTableContainer(this);
	m_nodeLookupTables.insert(name, c);
}

void Post2dBirdEyeWindowGridTypeDataItem::setupCellScalarsToColors(const std::string& name)
{
	LookupTableContainer* c = new LookupTableContainer(this);
	m_cellLookupTables.insert(name, c);
}
