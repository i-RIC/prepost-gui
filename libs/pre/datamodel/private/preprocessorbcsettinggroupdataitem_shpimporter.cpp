#include "preprocessorbcsettinggroupdataitem_shpimporter.h"
#include "../preprocessorbcdataitem.h"
#include "../preprocessorbcsettingdataitem.h"

#include <geodata/polygon/geodatapolygonregionpolygon.h>
#include <guicore/solverdef/solverdefinitionboundarycondition.h>
#include <misc/stringtool.h>

#include <string>
#include <unordered_map>

PreProcessorBCSettingGroupDataItem::ShpImporter::ShpImporter(const QString& fileName)
{
	m_shpHandle = SHPOpen(iRIC::toStr(fileName).c_str(), "rb");

	auto dbfName = fileName;
	dbfName.replace(".shp", ".dbf");
	m_dbfHandle = DBFOpen(iRIC::toStr(dbfName).c_str(), "rb");
}

PreProcessorBCSettingGroupDataItem::ShpImporter::~ShpImporter()
{
	SHPClose(m_shpHandle);
	DBFClose(m_dbfHandle);
}

void PreProcessorBCSettingGroupDataItem::ShpImporter::importPolygons(PreProcessorBCSettingGroupDataItem* item)
{
	std::unordered_map<std::string, PreProcessorBCSettingDataItem*> items;

	for (auto child : item->childItems()) {
		auto item = dynamic_cast<PreProcessorBCSettingDataItem*> (child);
		auto bcItem = item->bcDataItem();
		auto name = iRIC::toStr(QString("%1%2").arg(bcItem->condition()->name().c_str()).arg(bcItem->projectNumber()));

		items.insert({name, item});
	}

	int numEntities;
	int shapeType;
	double minBound[4];
	double maxBound[4];
	SHPGetInfo(m_shpHandle, &numEntities, &shapeType, minBound, maxBound);

	for (int i = 0; i < numEntities; ++i) {
		auto obj = SHPReadObject(m_shpHandle, i);
		if (obj == nullptr) {continue;}

		// it contains only one part
		QPolygonF polygon;

		for (int j = 0; j < obj->nVertices; ++j) {
			QPointF point(*(obj->padfX + j), *(obj->padfY + j));
			polygon.append(point);
		}
		if (polygon.at(0) != polygon.at(polygon.count() - 1)) {
			polygon.append(polygon.at(0));
		}

		std::string name = DBFReadStringAttribute(m_dbfHandle, i, 0);

		auto it = items.find(name);
		if (it == items.end()) {continue;}

		it->second->polygon()->regionPolygon()->setPolygon(polygon);
		it->second->polygon()->updatePolyData(true);
	}
}
