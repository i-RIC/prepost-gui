#include "../post3dwindow.h"
#include "post3dwindowgridtypedataitem.h"
#include "post3dwindowzonedataitem.h"

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

Post3dWindowGridTypeDataItem::Post3dWindowGridTypeDataItem(SolverDefinitionGridType* type, GraphicsWindowDataItem* parent)
	: Post3dWindowDataItem(type->caption(), parent)
{
	m_standardItem->setCheckable(true);
	m_standardItem->setCheckState(Qt::Checked);

	m_standardItemCopy = m_standardItem->clone();

	m_isDeletable = false;

	m_gridType = type;
	m_subFolder = type->name();
	m_isZoneDataItemsSetup = false;

	setupZoneDataItems();

	// add raw data node and grid data node.
	// setup ScalarsToColors instances
	QList<PostZoneDataContainer*> containers = postSolutionInfo()->zoneContainers3D();
	if (containers.size() != 0) {
		vtkPointSet* ps = containers.at(0)->data();
		if (ps == nullptr) {return;}
		vtkPointData* pd = ps->GetPointData();
		int num = pd->GetNumberOfArrays();
		for (int i = 0; i < num; ++i) {
			vtkAbstractArray* tmparray = pd->GetArray(i);
			if (tmparray == nullptr) {continue;}
			QString name = tmparray->GetName();

			setupScalarsToColors(name);
			setValueRange(name);
		}
	}
}

Post3dWindowGridTypeDataItem::~Post3dWindowGridTypeDataItem()
{
	for (auto z_it = m_zoneDatas.begin(); z_it != m_zoneDatas.end(); ++z_it) {
		delete *z_it;
	}
}

const QString& Post3dWindowGridTypeDataItem::name() const
{
	return m_gridType->name();
}

void Post3dWindowGridTypeDataItem::setupZoneDataItems()
{
	// first, clear the current zonedata.
	for (auto z_it = m_zoneDatas.begin(); z_it != m_zoneDatas.end(); ++z_it) {
		delete *z_it;
	}
	m_zoneDatas.clear();
	const QList<PostZoneDataContainer*>& zones = postSolutionInfo()->zoneContainers3D();
	int num = 0;
	int zoneNum = 0;
	for (auto it = zones.begin(); it != zones.end(); ++it) {
		const PostZoneDataContainer* cont = (*it);
		if (cont->data() == nullptr) {continue;}
		if (cont->gridType() == m_gridType) {
			Post3dWindowZoneDataItem* zdata = new Post3dWindowZoneDataItem(cont->zoneName(), num++, this);
			m_zoneDatas.append(zdata);
			m_zoneDataNameMap.insert(cont->zoneName(), zdata);
			m_childItems.append(zdata);
			++ zoneNum;
		}
	}
	if (m_lookupTables.count() == 0 && zoneNum != 0) {
		vtkPointSet* ps = zones.at(0)->data();
		if (ps != nullptr) {
			vtkPointData* pd = ps->GetPointData();
			int num = pd->GetNumberOfArrays();
			for (int i = 0; i < num; ++i) {
				vtkAbstractArray* tmparray = pd->GetArray(i);
				if (tmparray == nullptr) {continue;}
				QString name = tmparray->GetName();
				setupScalarsToColors(name);
			}
			updateLookupTableRanges();
		}
	}
	//+++++
	//assignActorZValues(m_zDepthRange);
	//+++++
	m_isZoneDataItemsSetup = (zoneNum != 0);
}

void Post3dWindowGridTypeDataItem::update()
{
	if (! m_isZoneDataItemsSetup) {
		setupZoneDataItems();
	}
	// update LookupTable range.
	updateLookupTableRanges();

	for (auto it = m_zoneDatas.begin(); it != m_zoneDatas.end(); ++it) {
		(*it)->update();
	}
}

void Post3dWindowGridTypeDataItem::updateLookupTableRanges()
{
	for (auto it = m_lookupTables.begin(); it != m_lookupTables.end(); ++it) {
		QString name = it.key();
		ScalarsToColorsContainer* cont = it.value();
		bool first = true;
		double range[2], min = 0, max = 0;
		for (auto zit = m_zoneDatas.begin(); zit != m_zoneDatas.end(); ++zit) {
			Post3dWindowZoneDataItem* zitem = *zit;
			PostZoneDataContainer* cont = zitem->dataContainer();
			if (cont == nullptr) {continue;}
			if (cont->data() == nullptr) {continue;}
			vtkDataArray* dArray = cont->data()->GetPointData()->GetArray(iRIC::toStr(name).c_str());
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

void Post3dWindowGridTypeDataItem::doLoadFromProjectMainFile(const QDomNode& node)
{
	QDomNode ltNode = iRIC::getChildNode(node, "LookupTables");
	if (! ltNode.isNull()) {
		QDomNodeList tables = ltNode.childNodes();
		for (int i = 0; i < tables.length(); ++i) {
			QDomNode ltNode = tables.at(i);
			QString ltName = ltNode.toElement().attribute("name");
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
			QString zoneName = zoneNode.toElement().attribute("name");
			Post3dWindowZoneDataItem* zdi = m_zoneDataNameMap.value(zoneName, nullptr);
			if (zdi != nullptr) {
				zdi->loadFromProjectMainFile(zoneNode);
			}
		}
	}
}

void Post3dWindowGridTypeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_gridType->name());
	writer.writeStartElement("LookupTables");
	for (auto lit = m_lookupTables.begin(); lit != m_lookupTables.end(); ++lit) {
		writer.writeStartElement("LookupTable");
		writer.writeAttribute("name", lit.key());
		LookupTableContainer* cont = lit.value();
		cont->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();
	writer.writeStartElement("Zones");
	for (auto zit = m_zoneDatas.begin(); zit != m_zoneDatas.end(); ++zit) {
		Post3dWindowZoneDataItem* zitem = *zit;
		writer.writeStartElement("Zone");
		zitem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();
}

void Post3dWindowGridTypeDataItem::setupScalarsToColors(const QString& name)
{
	LookupTableContainer* c = new LookupTableContainer(this);
	m_lookupTables.insert(name, c);
}

void Post3dWindowGridTypeDataItem::setValueRange(const QString& name)
{
	double min = -0.000001;
	double max = 0.000001;

	QList<PostZoneDataContainer*> containers = postSolutionInfo()->zoneContainers3D();

	bool first = true;
	for (auto it = containers.begin(); it != containers.end(); ++it) {
		vtkPointSet* ps = (*it)->data();
		if (ps == nullptr) { break; }
		double range[3];
		vtkDataArray* da = ps->GetPointData()->GetArray(iRIC::toStr(name).c_str());
		if (da == nullptr) { break; }
		da->GetRange(range);
		if (first || range[0] < min) {min = range[0];}
		if (first || range[1] > max) {max = range[1];}
		first = false;
	}
	// Now, new min, max are stored.
	m_lookupTables.value(name)->setValueRange(min, max);
}
