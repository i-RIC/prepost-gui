#ifndef GEODATAPOINTGROUPSHPEXPORTER_H
#define GEODATAPOINTGROUPSHPEXPORTER_H

#include "gd_pointgroup_global.h"

#include <guicore/pre/geodata/geodataexporter.h>

#include <shapefil.h>

class GeoDataPointGroupPoint;
class SolverDefinitionGridAttribute;

class GD_POINTGROUP_EXPORT GeoDataPointGroupShpExporter : public GeoDataExporter
{
public:
	GeoDataPointGroupShpExporter(GeoDataCreator* creator);

	bool doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;

private:
	SHPHandle getSHPHandle(QString filename);
	DBFHandle getDBFHandle(QString filename, SolverDefinitionGridAttribute* cond, bool* isDouble);
	SHPObject* getSHPObject(GeoDataPointGroupPoint* polygon, SHPHandle shp, int index, double xoffset = 0, double yoffset = 0);
	void outputAttributes(GeoDataPointGroupPoint* polygon, DBFHandle hDBF, int index, bool isDouble, QTextCodec* codec);
};

#endif // GEODATAPOINTGROUPSHPEXPORTER_H
