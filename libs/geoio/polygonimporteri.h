#ifndef POLYGONIMPORTERI_H
#define POLYGONIMPORTERI_H

#include "geoio_global.h"

class QPolygonF;
class QString;
class QStringList;
class QWidget;

class GEOIODLL_EXPORT PolygonImporterI
{
public:
	virtual ~PolygonImporterI() {}

	virtual QPolygonF importData(const QString& filename, const QString& selectedFilter, QWidget* w) = 0;
	virtual QStringList fileDialogFilters() = 0;
	virtual QStringList acceptableExtensions() = 0;
};

#endif // POLYGONIMPORTERI_H
