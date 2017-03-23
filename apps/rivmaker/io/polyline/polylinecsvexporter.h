#ifndef POLYLINECSVEXPORTER_H
#define POLYLINECSVEXPORTER_H

#include "polylineexporteri.h"

#include <QObject>

class PolyLineCsvExporter : public QObject, public PolyLineExporterI
{
	Q_OBJECT

public:
	QStringList fileDialogFilters() override;
	QStringList acceptableExtensions() override;
	bool exportData(const std::vector<QPointF>& points, const QPointF& offset, const QString& filename, QWidget* w) override;
};

#endif // POLYLINECSVEXPORTER_H
