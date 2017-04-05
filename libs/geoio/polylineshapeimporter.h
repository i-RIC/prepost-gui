#ifndef POLYLINESHAPEIMPORTER_H
#define POLYLINESHAPEIMPORTER_H

#include "geoio_global.h"

#include "polylineimporteri.h"

#include <QObject>

class GEOIODLL_EXPORT PolylineShapeImporter : public QObject, public PolylineImporterI
{
	Q_OBJECT

public:
	std::vector<QPointF> importData(const QString& filename, const QString& selectedFilter, QWidget* w) override;
	QStringList fileDialogFilters() override;
	QStringList acceptableExtensions() override;
};

#endif // POLYLINESHAPEIMPORTER_H
