#ifndef POLYLINECSVIMPORTER_H
#define POLYLINECSVIMPORTER_H

#include "polylineimporteri.h"

#include <QObject>

class PolyLineCsvImporter : public QObject, public PolyLineImporterI
{
	Q_OBJECT

public:
	QStringList fileDialogFilters() override;
	QStringList acceptableExtensions() override;
	bool importData(std::vector<QPointF>* points, QPointF* offset, const QString& filename, QWidget* w) override;
};

#endif // POLYLINECSVIMPORTER_H
