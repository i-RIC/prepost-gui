#ifndef POLYLINECSVIMPORTER_H
#define POLYLINECSVIMPORTER_H

#include "geoio_global.h"

#include "polylineimporteri.h"

#include <QObject>

class GEOIODLL_EXPORT PolylineCsvImporter : public QObject, public PolylineImporterI
{
	Q_OBJECT

public:
	QVector<QPointF> importData(const QString& filename, const QString& selectedFilter, QWidget* w) override;
	QStringList fileDialogFilters() override;
	QStringList acceptableExtensions() override;
};

#endif // POLYLINECSVIMPORTER_H
