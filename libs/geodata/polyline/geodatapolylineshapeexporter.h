#ifndef GEODATAPOLYLINESHAPEEXPORTER_H
#define GEODATAPOLYLINESHAPEEXPORTER_H

#include "gd_polyline_global.h"
#include <guicore/pre/geodata/geodataexporter.h>
#include <shapefil.h>

class GeoDataPolyLine;
class SolverDefinitionGridAttribute;

class GD_POLYLINE_EXPORT GeoDataPolyLineShapeExporter : public GeoDataExporter
{
	Q_OBJECT

public:
	GeoDataPolyLineShapeExporter(GeoDataCreator* creator);

	bool doExport(GeoData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;

	SHPHandle getSHPHandle(QString filename);
	DBFHandle getDBFHandle(QString filename, SolverDefinitionGridAttribute* cond, bool* isDouble);
	SHPObject* getSHPObject(GeoDataPolyLine* polyline, SHPHandle shp, int index, double xoffset = 0, double yoffset = 0);
	void outputAttributes(GeoDataPolyLine* polyline, DBFHandle hDBF, int index, bool isDouble, QTextCodec* codec);
};

#endif // GEODATAPOLYLINESHAPEEXPORTER_H
