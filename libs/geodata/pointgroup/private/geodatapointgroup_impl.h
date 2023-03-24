#ifndef GEODATAPOINTGROUP_IMPL_H
#define GEODATAPOINTGROUP_IMPL_H

#include "../geodatapointgroup.h"
#include "geodatapointgroup_setting.h"

#include <vector>

#include <QImage>

class vtkActor;
class vtkActor2D;
class vtkPoints;
class vtkPolyData;

class GeoDataPointGroup::Impl
{
public:
	Impl(GeoDataPointGroup* group);
	~Impl();

	static QPixmap shrinkPixmap(const QPixmap pixmap, int maxSize);
	static QString iconFileName(const QString& filename);
	void setupDummyPointForMenu();

	vtkPoints* m_vtkPoints;

	vtkPolyData* m_pointsPolyData;
	vtkActor* m_pointsActor;
	std::vector<vtkActor2D*> m_imageActors;

	vtkPolyData* m_selectedPointsPointsPolyData;
	vtkActor* m_selectedPointsPointsActor;

	GeoDataPoint* m_dummyPointForMenu;
	QImage m_shrinkedImage;

	Setting m_setting;
};

#endif // GEODATAPOINTGROUP_IMPL_H
