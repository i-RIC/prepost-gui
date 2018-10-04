#ifndef GEODATAPOLYGONSHAPEEXPORTER_H
#define GEODATAPOLYGONSHAPEEXPORTER_H

#include "gd_polygon_global.h"
#include <guicore/pre/geodata/geodataexporter.h>
#include <shapefil.h>

class GeoDataPolygon;
class SolverDefinitionGridAttribute;

class GD_POLYGON_EXPORT GeoDataPolygonShapeExporter : public GeoDataExporter
{
	Q_OBJECT

public:
	GeoDataPolygonShapeExporter(GeoDataCreator* creator);

	bool doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;

	SHPHandle getSHPHandle(QString filename);
	DBFHandle getDBFHandle(QString filename, SolverDefinitionGridAttribute* cond, bool* isDouble);
	SHPObject* getSHPObject(GeoDataPolygon* polygon, SHPHandle shp, int index, double xoffset = 0, double yoffset = 0);
	void outputAttributes(GeoDataPolygon* polygon, DBFHandle hDBF, int index, bool isDouble, QTextCodec* codec);
};

#endif // GEODATAPOLYGONSHAPEEXPORTER_H
