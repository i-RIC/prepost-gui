#include "polygoncsvimporter.h"
#include "polylinecsvimporter.h"
#include "polygonutil.h"

#include <QPolygonF>
#include <QStringList>

QPolygonF PolygonCsvImporter::importData(const QString& filename, const QString& selectedFilter, QWidget* w)
{
	QPolygonF ret, errorRet;

	PolylineCsvImporter importer;
	std::vector<QPointF> points = importer.importData(filename, selectedFilter, w);

	if (points.size() == 0) {
		return errorRet;
	}

	ret = PolygonUtil::fromStdVector(points);

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
