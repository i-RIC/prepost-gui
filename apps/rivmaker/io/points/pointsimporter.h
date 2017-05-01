#ifndef POINTSIMPORTER_H
#define POINTSIMPORTER_H

#include <QObject>

class GeometryPoint;

class PointsImporter : public QObject
{
	Q_OBJECT

public:
	static bool importData(std::vector<GeometryPoint*>* points, QPointF* offset, QWidget* w);

private:
	PointsImporter();
};

#endif // POINTSIMPORTER_H
