#ifndef POLYLINECSVEXPORTER_H
#define POLYLINECSVEXPORTER_H

#include "geoio_global.h"

#include "polylineexporteri.h"

#include <QObject>

class GEOIODLL_EXPORT PolylineCsvExporter : public QObject, public PolylineExporterI
{
	Q_OBJECT

public:
	void exportData(const std::vector<QPointF>& data, const QString& filename, const QString& selectedFilter, QWidget* w) override;
	QStringList fileDialogFilters() override;
};

#endif // POLYLINECSVEXPORTER_H
