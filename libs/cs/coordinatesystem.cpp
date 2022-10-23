#include "coordinatesystem.h"
#include "private/coordinatesystem_impl.h"

#include <misc/stringtool.h>

#include <ogr_spatialref.h>

#include <QFile>
#include <QRegExp>
#include <QTextStream>

CoordinateSystem::CoordinateSystem(const QString& name, const QString& caption, const QString& lonlat, const QString& plane) :
	impl {new Impl{}}
{
	impl->m_name = name;
	setCaption(caption);

	impl->m_proj4LonLatStr = lonlat;
	impl->m_proj4PlaneStr = plane;

	impl->m_proj4LonLatPj = nullptr;
	impl->m_proj4PlanePj = nullptr;

	impl->m_initialized = false;
}

CoordinateSystem::~CoordinateSystem()
{
	free();

	delete impl;
}

const QString& CoordinateSystem::name() const
{
	return impl->m_name;
}

void CoordinateSystem::setName(const QString& name)
{
	impl->m_name = name;
}

const QString& CoordinateSystem::caption() const
{
	return impl->m_caption;
}

void CoordinateSystem::setCaption(const QString& caption)
{
	impl->m_caption = "";
	if (! impl->m_name.isEmpty()) {
		impl->m_caption = impl->m_name;
		impl->m_caption.append(": ");
	}
	impl->m_caption.append(caption);
	impl->m_searchTarget = impl->m_caption.toLower();
}

const QString& CoordinateSystem::searchTarget() const
{
	return impl->m_searchTarget;
}

const QString& CoordinateSystem::proj4LonLatStr() const
{
	return impl->m_proj4LonLatStr;
}

const QString& CoordinateSystem::proj4PlaneStr() const
{
	return impl->m_proj4PlaneStr;
}


std::string CoordinateSystem::planeWkt() const
{
	QRegExp re("EPSG:(\\d+)");
	int pos = 0;
	pos = re.indexIn(name(), pos);
	if (pos == -1) {return "";}

	int epsgCode = re.cap(1).toInt();

	OGRSpatialReference ref;
	ref.importFromEPSG(epsgCode);

	char* buffer;
	ref.exportToWkt(&buffer);
	std::string ret = buffer;
	CPLFree(buffer);

	return ret;
}

bool CoordinateSystem::exportPlaneWkt(const QString& fname)
{
	QFile f(fname);
	bool ok = f.open(QFile::WriteOnly);
	if (! ok) {return false;}

	QTextStream stream(&f);
	stream << planeWkt().c_str();
	f.close();

	return true;
}

void CoordinateSystem::init()
{
	if (impl->m_initialized) {return;}

	impl->m_proj4LonLatPj = pj_init_plus(iRIC::toStr(impl->m_proj4LonLatStr).c_str());
	impl->m_proj4PlanePj = pj_init_plus(iRIC::toStr(impl->m_proj4PlaneStr).c_str());
}

void CoordinateSystem::free()
{
	if (! impl->m_initialized) {return;}

	pj_free(impl->m_proj4LonLatPj);
	pj_free(impl->m_proj4PlanePj);
}

void CoordinateSystem::mapGeoToGrid(double fromX, double fromY, double* toX, double* toY) const
{
	fromX *= DEG_TO_RAD;
	fromY *= DEG_TO_RAD;
	int ret = pj_transform(impl->m_proj4LonLatPj, impl->m_proj4PlanePj, 1, 1, &fromX, &fromY, NULL);
	if (ret == 0) {
		// success.
		*toX = fromX;
		*toY = fromY;
	}
}

void CoordinateSystem::mapGridToGeo(double fromX, double fromY, double* toX, double* toY) const
{
	int ret = pj_transform(impl->m_proj4PlanePj, impl->m_proj4LonLatPj, 1, 1, &fromX, &fromY, NULL);
	if (ret == 0) {
		fromX *= RAD_TO_DEG;
		fromY *= RAD_TO_DEG;
		// success.
		*toX = fromX;
		*toY = fromY;
	}
}

bool CoordinateSystem::isLongLat() const
{
	return impl->m_proj4PlaneStr.contains("+proj=longlat");
}
