#ifndef GEODATAPOLYLINEGROUPSHPEXPORTER_H
#define GEODATAPOLYLINEGROUPSHPEXPORTER_H

#include "gd_polylinegroup_global.h"

#include <guicore/pre/geodata/geodataexporter.h>

#include <shapefil.h>

class GeoDataPolyLineGroupPolyLine;
class SolverDefinitionGridAttribute;

class GD_POLYLINEGROUP_EXPORT GeoDataPolyLineGroupShpExporter : public GeoDataExporter
{
	Q_OBJECT

public:
	GeoDataPolyLineGroupShpExporter(GeoDataCreator* creator);

	bool doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;

private:
	SHPHandle getSHPHandle(QString filename);
	DBFHandle getDBFHandle(QString filename, SolverDefinitionGridAttribute* cond, bool* isDouble);
	SHPObject* getSHPObject(GeoDataPolyLineGroupPolyLine* polygon, SHPHandle shp, int index, double xoffset = 0, double yoffset = 0);
	void outputAttributes(GeoDataPolyLineGroupPolyLine* polygon, DBFHandle hDBF, int index, bool isDouble, QTextCodec* codec);
};

#endif // GEODATAPOLYLINEGROUPSHPEXPORTER_H
