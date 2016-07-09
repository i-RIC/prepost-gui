#include "polygoncsvimporter.h"
#include "polylinecsvimporter.h"

#include <QPolygonF>

QPolygonF PolygonCsvImporter::importData(const QString& filename, const QString& selectedFilter, QWidget* w)
{
	QPolygonF ret, errorRet;

	PolylineCsvImporter importer;
	QVector<QPointF> points = importer.importData(filename, selectedFilter, w);

	if (points.isEmpty()) {
		return errorRet;
	}

	ret << points;

	if (ret.first() != ret.last()) {
		ret.push_back(ret.first());
	}

	return ret;
}

QStringList PolygonCsvImporter::fileDialogFilters()
{
	QStringList ret;
	ret << tr("CSV file (*.csv)");

	return ret;
}

QStringList PolygonCsvImporter::acceptableExtensions()
{
	QStringList ret;
	ret << "csv";

	return ret;
}
