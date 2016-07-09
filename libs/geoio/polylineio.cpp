#include "io.h"
#include "polylineexporterfactory.h"
#include "polylineexporteri.h"
#include "polylineimporterfactory.h"
#include "polylineimporteri.h"
#include "polylineio.h"

QVector<QPointF> PolylineIO::importData(QWidget* w)
{
	return importDataT<PolylineImporterFactory, QVector<QPointF>, PolylineImporterI> (w);
}

void PolylineIO::exportData(const QVector<QPointF>& polyline, QWidget* w)
{
	exportDataT<PolylineExporterFactory, QVector<QPointF>, PolylineExporterI> (polyline, w);
}

PolylineIO::PolylineIO()
{}
