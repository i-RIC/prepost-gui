#include "io.h"
#include "polylineexporterfactory.h"
#include "polylineexporteri.h"
#include "polylineimporterfactory.h"
#include "polylineimporteri.h"
#include "polylineio.h"

std::vector<QPointF> PolylineIO::importData(QWidget* w)
{
	return importDataT<PolylineImporterFactory, std::vector<QPointF>, PolylineImporterI> (w);
}

void PolylineIO::exportData(const std::vector<QPointF>& polyline, QWidget* w)
{
	exportDataT<PolylineExporterFactory, std::vector<QPointF>, PolylineExporterI> (polyline, w);
}

PolylineIO::PolylineIO()
{}
