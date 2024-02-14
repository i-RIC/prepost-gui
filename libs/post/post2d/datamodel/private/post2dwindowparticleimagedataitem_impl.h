#ifndef POST2DWINDOWPARTICLEIMAGEDATAITEM_IMPL_H
#define POST2DWINDOWPARTICLEIMAGEDATAITEM_IMPL_H

#include "../post2dwindowparticleimagedataitem.h"
#include "post2dwindowparticleimagedataitem_setting.h"

#include <vector>

class vtkActor;
class vtkPolyData;
class vtkTexture;

class Post2dWindowParticleImageDataItem::Impl
{
public:
	Impl();
	~Impl();

	vtkPoints* buildCornerPoints(const QPointF& pos, double value, double angle);
	QPointF buildBottomLeftCorner(const QPointF& pos, double value, double angle);
	double calcScale(double value);
	double calcSize(double value);

	vtkTexture* buildTexture();
	vtkPolyData* buildPlane();

	std::vector<vtkActor*> m_actors;

	Setting m_setting;

private:
	std::vector<QPointF> buildCornerPointsCenter(const QPointF& point, const QPointF& dx, const QPointF& dy, double halfWidth, double halfHeight);
	std::vector<QPointF> buildCornerPointsTopLeft(const QPointF& point, const QPointF& dx, const QPointF& dy, double halfWidth, double halfHeight);
	std::vector<QPointF> buildCornerPointsTop(const QPointF& point, const QPointF& dx, const QPointF& dy, double halfWidth, double halfHeight);
	std::vector<QPointF> buildCornerPointsTopRight(const QPointF& point, const QPointF& dx, const QPointF& dy, double halfWidth, double halfHeight);
	std::vector<QPointF> buildCornerPointsLeft(const QPointF& point, const QPointF& dx, const QPointF& dy, double halfWidth, double halfHeight);
	std::vector<QPointF> buildCornerPointsRight(const QPointF& point, const QPointF& dx, const QPointF& dy, double halfWidth, double halfHeight);
	std::vector<QPointF> buildCornerPointsBottomLeft(const QPointF& point, const QPointF& dx, const QPointF& dy, double halfWidth, double halfHeight);
	std::vector<QPointF> buildCornerPointsBottom(const QPointF& point, const QPointF& dx, const QPointF& dy, double halfWidth, double halfHeight);
	std::vector<QPointF> buildCornerPointsBottomRight(const QPointF& point, const QPointF& dx, const QPointF& dy, double halfWidth, double halfHeight);
};

#endif // POST2DWINDOWPARTICLEIMAGEDATAITEM_IMPL_H
