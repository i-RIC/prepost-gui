#ifndef POLYLINEIMPORTERI_H
#define POLYLINEIMPORTERI_H

#include "geoio_global.h"

#include <QVector>

class QPointF;
class QString;
class QStringList;
class QWidget;

class GEOIODLL_EXPORT PolylineImporterI
{
public:
	virtual ~PolylineImporterI() {}

	virtual QVector<QPointF> importData(const QString& filename, const QString& selectedFilter, QWidget* w) = 0;
	virtual QStringList fileDialogFilters() = 0;
	virtual QStringList acceptableExtensions() = 0;
};

#endif // POLYLINEIMPORTERI_H
