#include "points.h"
#include "pointspreprocessorcontroller.h"
#include "../project/project.h"
#include "../../geom/geometrypoint.h"

#include "private/points_impl.h"

#include <QFile>
#include <QDataStream>

Points::Points(DataItem* parent) :
	DataItem {parent},
	impl {new Impl {}}
{}

Points::~Points()
{
	clearPoints();
	delete impl;
}

const std::vector<GeometryPoint*> &Points::points() const
{
	return impl->m_points;
}

std::vector<GeometryPoint*> &Points::points()
{
	return impl->m_points;
}

void Points::setPoints(const std::vector<GeometryPoint*> &points)
{
	clearPoints();
	impl->m_points = points;
	project()->setModified();
}

void Points::clearPoints()
{
	for (auto p : impl->m_points) {
		delete p;
	}
	impl->m_points.clear();
	project()->setModified();
}

DataItemController* Points::buildPreProcessorDataItemController(Model* model)
{
	return new PointsPreProcessorController(model, this);
}

void Points::loadExternalData(const QString& filename)
{
	QFile file(filename);
	bool ok = file.open(QIODevice::ReadOnly);
	if (! ok) {return;}

	QDataStream in(&file);
	in.setVersion(QDATASTREAM_VERSION);

	qint32 count;
	in >> count;

	for (qint32 i = 0; i < count; ++i) {
		float x, y, z;
		QString name;
		in >> x >> y >> z >> name;
		GeometryPoint* p = new GeometryPoint(x, y, z, name);
		impl->m_points.push_back(p);
	}
	file.close();
}

void Points::saveExternalData(const QString& filename) const
{
	QFile file(filename);
	bool ok = file.open(QIODevice::WriteOnly);
	if (! ok) {return;}
	QDataStream out(&file);
	out.setVersion(QDATASTREAM_VERSION);

	qint32 count = static_cast<qint32>(impl->m_points.size());
	out << count;

	for (GeometryPoint* p : impl->m_points) {
		out << p->x() << p->y() << p->z() << p->name();
	}

	file.close();
}
