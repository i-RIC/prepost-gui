#ifndef POLYLINEEXPORTERI_H
#define POLYLINEEXPORTERI_H

#include "geoio_global.h"

#include <vector>

class QPointF;
class QString;
class QStringList;
class QWidget;

class GEOIODLL_EXPORT PolylineExporterI
{
public:
	virtual ~PolylineExporterI() {}

	virtual void exportData(const std::vector<QPointF>& data, const QString& filename, const QString& selectedFilter, QWidget* w) = 0;
	virtual QStringList fileDialogFilters() = 0;
};

#endif // POLYLINEEXPORTERI_H
