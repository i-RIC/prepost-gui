#ifndef GEODATAPOLYGONGROUPSHPEXPORTER_H
#define GEODATAPOLYGONGROUPSHPEXPORTER_H

#include "gd_polygongroup_global.h"

#include <guicore/pre/geodata/geodataexporter.h>

#include <shapefil.h>

class GeoDataPolygonGroupPolygon;

class GD_POLYGONGROUP_EXPORT GeoDataPolygonGroupShpExporter : public GeoDataExporter
{
public:
	GeoDataPolygonGroupShpExporter(GeoDataCreator* creator);

	bool doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;

private:
	SHPHandle getSHPHandle(QString filename);
	DBFHandle getDBFHandle(QString filename, SolverDefinitionGridAttribute* cond, bool* isDouble);
	SHPObject* getSHPObject(GeoDataPolygonGroupPolygon* polygon, SHPHandle shp, int index, double xoffset = 0, double yoffset = 0);
	void outputAttributes(GeoDataPolygonGroupPolygon* polygon, DBFHandle hDBF, int index, bool isDouble, QTextCodec* codec);
};

#endif // GEODATAPOLYGONGROUPSHPEXPORTER_H
