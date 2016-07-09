#ifndef POLYGONSHAPEIMPORTER_H
#define POLYGONSHAPEIMPORTER_H

#include "geoio_global.h"

#include "polygonimporteri.h"

#include <QObject>

class GEOIODLL_EXPORT PolygonShapeImporter : public QObject, public PolygonImporterI
{
	Q_OBJECT

public:
	QPolygonF importData(const QString& filename, const QString& selectedFilter, QWidget* w) override;
	QStringList fileDialogFilters() override;
	QStringList acceptableExtensions() override;
};

#endif // POLYGONSHAPEIMPORTER_H
