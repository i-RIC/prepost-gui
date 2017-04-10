#ifndef POLYLINEEXPORTERI_H
#define POLYLINEEXPORTERI_H

#include <vector>

class QPointF;
class QString;
class QStringList;
class QWidget;

class PolyLineExporterI
{
public:
	virtual ~PolyLineExporterI() {}

	virtual QStringList fileDialogFilters() = 0;
	virtual QStringList acceptableExtensions() = 0;
	virtual bool exportData(const std::vector<QPointF>& points, const QPointF& offset, const QString& filename, QWidget* w) = 0;
};

#endif // POLYLINEEXPORTERI_H
