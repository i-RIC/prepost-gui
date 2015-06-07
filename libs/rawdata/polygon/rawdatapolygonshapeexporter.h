#ifndef RAWDATAPOLYGONSHAPEEXPORTER_H
#define RAWDATAPOLYGONSHAPEEXPORTER_H

#include "rd_polygon_global.h"
#include <guicore/pre/rawdata/rawdataexporter.h>
#include <shapefil.h>

class RawDataPolygon;
class SolverDefinitionGridAttribute;

class RD_POLYGON_EXPORT RawDataPolygonShapeExporter : public RawDataExporter
{
	Q_OBJECT

public:
	RawDataPolygonShapeExporter(RawDataCreator* creator);
	bool doExport(RawData* data, const QString& filename, const QString& selectedFilter, QWidget* w, ProjectData* pd) override;
	const QStringList fileDialogFilters() override;
	SHPHandle getSHPHandle(QString filename);
	DBFHandle getDBFHandle(QString filename, SolverDefinitionGridAttribute* cond, bool* isDouble);
	SHPObject* getSHPObject(RawDataPolygon* polygon, SHPHandle shp, int index, double xoffset = 0, double yoffset = 0);
	void outputAttributes(RawDataPolygon* polygon, DBFHandle hDBF, int index, bool isDouble, QTextCodec* codec);
};

#endif // RAWDATAPOLYGONSHAPEEXPORTER_H
