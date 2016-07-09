#ifndef POLYGONCSVEXPORTER_H
#define POLYGONCSVEXPORTER_H

#include "geoio_global.h"

#include "polygonexporteri.h"

#include <QObject>

class GEOIODLL_EXPORT PolygonCsvExporter : public QObject, public PolygonExporterI
{
	Q_OBJECT

public:
	void exportData(const QPolygonF& data, const QString& filename, const QString& selectedFilter, QWidget* w) override;
	QStringList fileDialogFilters() override;
};

#endif // POLYGONCSVEXPORTER_H
