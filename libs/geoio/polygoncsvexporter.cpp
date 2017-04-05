#include "polygoncsvexporter.h"
#include "polygonutil.h"
#include "polylinecsvexporter.h"

#include <QPolygonF>
#include <QStringList>


void PolygonCsvExporter::exportData(const QPolygonF& data, const QString& filename, const QString& selectedFilter, QWidget* w)
{
	PolylineCsvExporter exporter;
	exporter.exportData(PolygonUtil::toStdVector(data), filename, selectedFilter, w);
}

QStringList PolygonCsvExporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("CSV file (*.csv)");

	return ret;
}
