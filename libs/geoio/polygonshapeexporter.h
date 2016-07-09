#ifndef POLYGONSHAPEEXPORTER_H
#define POLYGONSHAPEEXPORTER_H

#include "geoio_global.h"

#include "polygonexporteri.h"

#include <QObject>

class GEOIODLL_EXPORT PolygonShapeExporter : public QObject, public PolygonExporterI
{
	Q_OBJECT

public:
	void exportData(const QPolygonF& data, const QString& filename, const QString& selectedFilter, QWidget* w) override;
	QStringList fileDialogFilters() override;
};

#endif // POLYGONSHAPEEXPORTER_H
