#include "io.h"
#include "polygonexporterfactory.h"
#include "polygonexporteri.h"
#include "polygonimporterfactory.h"
#include "polygonimporteri.h"
#include "polygonio.h"

QPolygonF PolygonIO::importData(QWidget* w)
{
	return importDataT<PolygonImporterFactory, QPolygonF, PolygonImporterI> (w);
}

void PolygonIO::exportData(const QPolygonF& polygon, QWidget* w)
{
	exportDataT<PolygonExporterFactory, QPolygonF, PolygonExporterI> (polygon, w);
}

PolygonIO::PolygonIO()
{}
