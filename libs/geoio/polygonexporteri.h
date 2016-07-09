#ifndef POLYGONEXPORTERI_H
#define POLYGONEXPORTERI_H

#include "geoio_global.h"

class QPolygonF;
class QString;
class QStringList;
class QWidget;

class GEOIODLL_EXPORT PolygonExporterI
{
public:
	virtual ~PolygonExporterI() {}

	virtual void exportData(const QPolygonF& data, const QString& filename, const QString& selectedFilter, QWidget* w) = 0;
	virtual QStringList fileDialogFilters() = 0;
};

#endif // POLYGONEXPORTERI_H
