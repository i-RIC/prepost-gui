#ifndef POINTSCSVIMPORTER_H
#define POINTSCSVIMPORTER_H

#include "pointsimporteri.h"

#include <QObject>

class PointsCsvImporter : public QObject, public PointsImporterI
{
	Q_OBJECT

public:
	QStringList fileDialogFilters() override;
	QStringList acceptableExtensions() override;
	bool importData(std::vector<GeometryPoint*>* points, QPointF* offset, const QString& filename, QWidget* w) override;
};

#endif // POINTSCSVIMPORTER_H
