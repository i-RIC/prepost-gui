#include "preprocessorbcsettinggroupdataitem_shpexporter.h"
#include "../preprocessorbcdataitem.h"
#include "../preprocessorbcsettingdataitem.h"

#include <geodata/polygon/geodatapolygon.h>
#include <geodata/polygon/geodatapolygonregionpolygon.h>
#include <guicore/project/projectdata.h>
#include <guicore/project/projectmainfile.h>
#include <guicore/solverdef/solverdefinitionboundarycondition.h>
#include <misc/stringtool.h>

PreProcessorBCSettingGroupDataItem::ShpExporter::ShpExporter(const QString& fileName) :
	m_index {0}
{
	m_shpHandle = SHPCreate(iRIC::toStr(fileName).c_str(), SHPT_POLYGON);

	auto dbfName = fileName;
	dbfName.replace(".shp", ".dbf");
	m_dbfHandle = DBFCreate(iRIC::toStr(dbfName).c_str());

	DBFAddField(m_dbfHandle, "Name", FTString, 100, 0);
}

PreProcessorBCSettingGroupDataItem::ShpExporter::~ShpExporter()
{
	SHPClose(m_shpHandle);
	DBFClose(m_dbfHandle);
}

void PreProcessorBCSettingGroupDataItem::ShpExporter::exportPolygon(PreProcessorBCSettingDataItem* item)
{
	auto pol = item->polygon()->regionPolygon()->polygon(item->projectData()->mainfile()->offset());
	if (pol.size() == 0) {return;}

	std::vector<double> padfX, padfY;
	for (int i = 0; i < pol.size(); ++i) {
		padfX.push_back(pol.at(i).x());
		padfY.push_back(pol.at(i).y());
	}

	auto polygon = SHPCreateSimpleObject(SHPT_POLYGON, pol.size(), padfX.data(), padfY.data(), NULL);
	SHPComputeExtents(polygon);
	SHPRewindObject(m_shpHandle, polygon);
	SHPWriteObject(m_shpHandle, -1, polygon);

	auto bcItem = item->bcDataItem();
	auto name = QString("%1%2").arg(bcItem->condition()->name().c_str()).arg(bcItem->projectNumber());

	DBFWriteStringAttribute(m_dbfHandle, m_index, 0, iRIC::toStr(name).c_str());

	++ m_index;
}
