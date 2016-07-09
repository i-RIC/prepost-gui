#ifndef POLYLINESHAPEEXPORTER_H
#define POLYLINESHAPEEXPORTER_H

#include "geoio_global.h"

#include "polylineexporteri.h"

#include <QObject>

class GEOIODLL_EXPORT PolylineShapeExporter : public QObject, public PolylineExporterI
{
	Q_OBJECT

public:
	void exportData(const QVector<QPointF>& data, const QString& filename, const QString& selectedFilter, QWidget* w) override;
	QStringList fileDialogFilters() override;
};


#endif // POLYLINESHAPEEXPORTER_H
