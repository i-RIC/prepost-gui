#ifndef GEODATAPOINTGROUP_IMPL_H
#define GEODATAPOINTGROUP_IMPL_H

#include "../geodatapointgroup.h"
#include "geodatapointgroup_displaysetting.h"

#include <vector>

#include <QImage>

class VTKGraphicsView;
class VTK2DGraphicsView;

class vtkActor;
class vtkActor2D;
class vtkPoints;
class vtkPolyData;

class GeoDataPointGroup::Impl
{
public:
	Impl(GeoDataPointGroup* group);
	~Impl();

	static double calcImageScale(double scale, const DisplaySetting& s);
	static QPixmap shrinkPixmap(const QPixmap& pixmap, const DisplaySetting& s, VTKGraphicsView* view);
	static QPixmap shrinkPixmap(const QPixmap& pixmap, int maxSize);
	static QPointF buildBottomLeftCorner(const QPointF& pos, const QPixmap& pixmap, DisplaySetting::AnchorPosition anchor, VTK2DGraphicsView* view);

	void setupDummyPointForMenu();

	vtkPoints* m_vtkPoints;

	vtkPolyData* m_pointsPolyData;
	vtkActor* m_pointsActor;
	std::vector<vtkActor2D*> m_imageActors;

	vtkPolyData* m_selectedPointsPointsPolyData;
	vtkActor* m_selectedPointsPointsActor;

	GeoDataPoint* m_dummyPointForMenu;
	QImage m_shrinkedImage;

	DisplaySetting m_displaySetting;

private:
	GeoDataPointGroup* m_group;
};

#endif // GEODATAPOINTGROUP_IMPL_H
