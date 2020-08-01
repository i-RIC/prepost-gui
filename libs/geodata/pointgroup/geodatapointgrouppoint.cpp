#include "geodatapointgroup.h"
#include "geodatapointgrouppoint.h"
#include "private/geodatapointgrouppoint_impl.h"

#include <geodata/point/geodatapoint.h>

#include <QDataStream>
#include <QList>
#include <QPointF>
#include <QVector>

using namespace geos::geom;

GeoDataPointGroupPoint::GeoDataPointGroupPoint(GeoDataPointGroup* group) :
	GeoDataPolyDataGroupPolyData(group),
	impl {new Impl {}}
{}

GeoDataPointGroupPoint::GeoDataPointGroupPoint(const QPointF& point, GeoDataPointGroup* group) :
	GeoDataPointGroupPoint(group)
{
	impl->m_point = point;
}

GeoDataPointGroupPoint::~GeoDataPointGroupPoint()
{
	delete impl;
}

QPointF GeoDataPointGroupPoint::point() const
{
	return impl->m_point;
}

void GeoDataPointGroupPoint::setPoint(const QPointF& point)
{
	impl->m_point = point;
}

void GeoDataPointGroupPoint::getBoundingRect(double* xmin, double* xmax, double* ymin, double* ymax)
{
	auto p = point();
	*xmin = p.x();
	*xmax = p.x();
	*ymin = p.y();
	*ymax = p.y();
}

void GeoDataPointGroupPoint::copyShapeFrom(GeoDataPolyData* data)
{
	auto p = dynamic_cast<GeoDataPoint*> (data);
	setPoint(p->point());
}

void GeoDataPointGroupPoint::copyShapeTo(GeoDataPolyData* data)
{
	auto p = dynamic_cast<GeoDataPoint*> (data);
	p->setPoint(point());
	p->setMouseEventMode(GeoDataPoint::meNormal);
}

void GeoDataPointGroupPoint::loadExternalData(QDataStream* stream)
{
	QString id;
	QString name;
	QVariant value;

	*stream >> id >> name >> value;
	setId(id);
	setName(name);
	setValue(value);

	QPointF point;
	*stream >> point;

	impl->m_point = point;
}

void GeoDataPointGroupPoint::saveExternalData(QDataStream* stream)
{
	*stream << id();
	*stream << name();
	*stream << value();

	*stream << impl->m_point;
}

void GeoDataPointGroupPoint::applyOffset(double x, double y)
{
	setPoint(point() + QPointF(x, y));
}
