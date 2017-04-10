#ifndef POLYLINEIMPORTERI_H
#define POLYLINEIMPORTERI_H

#include <vector>

class QPointF;
class QString;
class QStringList;
class QWidget;

class PolyLineImporterI
{
public:
	virtual ~PolyLineImporterI() {}

	virtual QStringList fileDialogFilters() = 0;
	virtual QStringList acceptableExtensions() = 0;
	virtual bool importData(std::vector<QPointF>* points, QPointF* offset, const QString& filename, QWidget* w) = 0;
};

#endif // POLYLINEIMPORTERI_H
