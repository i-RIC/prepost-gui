#ifndef POINTSIMPORTERI_H
#define POINTSIMPORTERI_H

#include <vector>

class QPointF;
class QString;
class QStringList;
class QVector3D;
class QWidget;

class PointsImporterI
{
public:
	virtual ~PointsImporterI() {}

	virtual QStringList fileDialogFilters() = 0;
	virtual QStringList acceptableExtensions() = 0;
	virtual bool importData(std::vector<QVector3D*>* points, QPointF* offset, const QString& filename, QWidget* w) = 0;
};

#endif // POINTSIMPORTERI_H
