#include "post2dbirdeyewindowgridtypedataitem.h"
#include <guicore/solverdef/solverdefinitiongridtype.h>
#include <guicore/solverdef/solverdefinitiongridrelatedcondition.h>
#include "post2dbirdeyewindowzonedataitem.h"
#include "../post2dbirdeyewindow.h"
#include <guicore/scalarstocolors/lookuptablecontainer.h>
#include <guicore/pre/gridcond/base/gridrelatedconditioncontainer.h>
#include <guicore/pre/grid/grid.h>
#include <guicore/postcontainer/postsolutioninfo.h>
#include <guicore/postcontainer/postzonedatacontainer.h>
#include <misc/xmlsupport.h>
#include <misc/stringtool.h>
#include <QMenu>
#include <QList>
#include <QAction>
#include <QStandardItem>
#include <QXmlStreamWriter>
#include <vtkPointData.h>

Post2dBirdEyeWindowGridTypeDataItem::Post2dBirdEyeWindowGridTypeDataItem(SolverDefinitionGridType* type, GraphicsWindowDataItem* parent)
	: Post2dBirdEyeWindowDataItem(type->caption(), QIcon(":/libs/guibase/images/iconFolder.png"), parent)
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
	QList<PostZoneDataContainer*> containers = postSolutionInfo()->zoneContainers2D();
	if (containers.size() != 0){
		vtkPointData* pd = containers.at(0)->data()->GetPointData();
		int num = pd->GetNumberOfArrays();
		for (int i = 0; i < num; ++i){
			vtkAbstractArray* tmparray = pd->GetArray(i);
			if (tmparray == 0){continue;}
			QString name = tmparray->GetName();
			setupScalarsToColors(name);
		}
		updateLookupTableRanges();
	}
}

Post2dBirdEyeWindowGridTypeDataItem::~Post2dBirdEyeWindowGridTypeDataItem()
{
	QList<Post2dBirdEyeWindowZoneDataItem*>::iterator z_it;
	for(z_it = m_zoneDatas.begin(); z_it != m_zoneDatas.end(); ++z_it){
		delete *z_it;
	}
}

const QString& Post2dBirdEyeWindowGridTypeDataItem::name()
{
	return m_gridType->name();
}

void Post2dBirdEyeWindowGridTypeDataItem::setupZoneDataItems()
{
	// first, clear the current zonedata.
	QList<Post2dBirdEyeWindowZoneDataItem*>::iterator z_it;
	for (z_it = m_zoneDatas.begin(); z_it != m_zoneDatas.end(); ++z_it){
		delete *z_it;
	}
	m_zoneDatas.clear();
	const QList<PostZoneDataContainer*>& zones = postSolutionInfo()->zoneContainers2D();
	QList<PostZoneDataContainer*>::const_iterator it;
	int num = 0;
	int zoneNum = 0;
	for (it = zones.begin(); it != zones.end(); ++it){
		const PostZoneDataContainer* cont = (*it);
		if (cont->gridType() == m_gridType){
			Post2dBirdEyeWindowZoneDataItem* zdata = new Post2dBirdEyeWindowZoneDataItem(cont->zoneName(), num++, this);
			m_zoneDatas.append(zdata);
			m_zoneDataNameMap.insert(cont->zoneName(), zdata);
			m_childItems.append(zdata);
			++ zoneNum;
		}
	}
	if (m_lookupTables.count() == 0 && zones.size() != 0){
		vtkPointData* pd = zones.at(0)->data()->GetPointData();
		int num = pd->GetNumberOfArrays();
		for(int i = 0; i < num; ++i){
			vtkAbstractArray* tmparray = pd->GetArray(i);
			if (tmparray == 0){continue;}
			QString name = tmparray->GetName();
			setupScalarsToColors(name);
		}
		updateLookupTableRanges();
	}
	assignActionZValues(m_zDepthRange);
	m_isZoneDataItemsSetup = (zoneNum != 0);
}

void Post2dBirdEyeWindowGridTypeDataItem::update()
{
	if (! m_isZoneDataItemsSetup)
	{
		setupZoneDataItems();
	}
	// update LookupTable range.
	updateLookupTableRanges();

	// update child items.
	QList<Post2dBirdEyeWindowZoneDataItem*>::iterator it;
	for (it = m_zoneDatas.begin(); it != m_zoneDatas.end(); ++it){
		(*it)->update();
	}
}

void Post2dBirdEyeWindowGridTypeDataItem::updateLookupTableRanges()
{
	QMap<QString, LookupTableContainer*>::iterator it;
	for (it = m_lookupTables.begin(); it != m_lookupTables.end(); ++it){
		QString name = it.key();
		ScalarsToColorsContainer* cont = it.value();
		QList<Post2dBirdEyeWindowZoneDataItem*>::iterator zit;
		bool first = true;
		double range[2], min, max;
		min = 0; max = 0;
		for (zit = m_zoneDatas.begin(); zit != m_zoneDatas.end(); ++zit){
			Post2dBirdEyeWindowZoneDataItem* zitem = *zit;
			if (zitem->dataContainer() == 0 || zitem->dataContainer()->data() == 0){continue;}
			vtkDataArray* dArray = zitem->dataContainer()->data()->GetPointData()->GetArray(iRIC::toStr(name).c_str());
			if (dArray != 0){
				dArray->GetRange(range);
				if (first || range[0] < min){min = range[0];}
				if (first || range[1] > max){max = range[1];}
				first = false;
			}
		}
		if (max - min < 1E-4){
			// the width is too small.
			double mid = (min + max) * 0.5;
			double width = mid * 0.01;
			if (width < 1E-4){
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
	if (! ltNode.isNull()){
		QDomNodeList tables = ltNode.childNodes();
		for (int i = 0; i < tables.length(); ++i){
			QDomNode ltNode = tables.at(i);
			QString ltName = ltNode.toElement().attribute("name");
			LookupTableContainer* cont = m_lookupTables.value(ltName, 0);
			if (cont != 0){
				cont->loadFromProjectMainFile(ltNode);
			}
		}
	}
	QDomNode zonesNode = iRIC::getChildNode(node, "Zones");
	if (! zonesNode.isNull()){
		QDomNodeList zones = zonesNode.childNodes();
		for (int i = 0; i < zones.length(); ++i){
			QDomNode zoneNode = zones.at(i);
			QString zoneName = zoneNode.toElement().attribute("name");
			Post2dBirdEyeWindowZoneDataItem* zdi = m_zoneDataNameMap.value(zoneName, 0);
			if (zdi != 0){
				zdi->loadFromProjectMainFile(zoneNode);
			}
		}
	}
}

void Post2dBirdEyeWindowGridTypeDataItem::doSaveToProjectMainFile(QXmlStreamWriter& writer)
{
	writer.writeAttribute("name", m_gridType->name());
	writer.writeStartElement("LookupTables");
	QMap<QString, LookupTableContainer*>::iterator lit;
	for (lit = m_lookupTables.begin(); lit != m_lookupTables.end(); ++lit) {
		writer.writeStartElement("LookupTable");
		writer.writeAttribute("name", lit.key());
		LookupTableContainer* cont = lit.value();
		cont->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();
	writer.writeStartElement("Zones");
	QList<Post2dBirdEyeWindowZoneDataItem*>::iterator zit;
	for (zit = m_zoneDatas.begin(); zit != m_zoneDatas.end(); ++zit){
		Post2dBirdEyeWindowZoneDataItem* zitem = *zit;
		writer.writeStartElement("Zone");
		zitem->saveToProjectMainFile(writer);
		writer.writeEndElement();
	}
	writer.writeEndElement();
}

void Post2dBirdEyeWindowGridTypeDataItem::setupScalarsToColors(const QString& name)
{
	LookupTableContainer* c = new LookupTableContainer(this);
	m_lookupTables.insert(name, c);
}
