#include "coordinatesystem.h"

#include <misc/stringtool.h>

CoordinateSystem::CoordinateSystem(const QString& name, const QString& caption, const QString& lonlat, const QString& plane)
{
	m_name = name;
	setCaption(caption);

	m_proj4LonLatStr = lonlat;
	m_proj4PlaneStr = plane;

	m_proj4LonLatPj = 0;
	m_proj4PlanePj = 0;

	m_initialized = false;
}


CoordinateSystem::~CoordinateSystem()
{
	free();
}

void CoordinateSystem::setCaption(const QString& caption)
{
	m_caption = "";
	if (! m_name.isEmpty()) {
		m_caption = m_name;
		m_caption.append(": ");
	}
	m_caption.append(caption);
	m_searchTarget = m_caption.toLower();
}

void CoordinateSystem::init()
{
	if (m_initialized) {return;}

	m_proj4LonLatPj = pj_init_plus(iRIC::toStr(m_proj4LonLatStr).c_str());
	m_proj4PlanePj = pj_init_plus(iRIC::toStr(m_proj4PlaneStr).c_str());
}


void CoordinateSystem::free()
{
	if (! m_initialized) {return;}

	pj_free(m_proj4LonLatPj);
	pj_free(m_proj4PlanePj);
}

void CoordinateSystem::mapGeoToGrid(double fromX, double fromY, double* toX, double* toY)
{
	fromX *= DEG_TO_RAD;
	fromY *= DEG_TO_RAD;
	int ret = pj_transform(m_proj4LonLatPj, m_proj4PlanePj, 1, 1, &fromX, &fromY, NULL);
	if (ret == 0) {
		// success.
		*toX = fromX;
		*toY = fromY;
	}
}

void CoordinateSystem::mapGridToGeo(double fromX, double fromY, double* toX, double* toY)
{
	int ret = pj_transform(m_proj4PlanePj, m_proj4LonLatPj, 1, 1, &fromX, &fromY, NULL);
	if (ret == 0) {
		fromX *= RAD_TO_DEG;
		fromY *= RAD_TO_DEG;
		// success.
		*toX = fromX;
		*toY = fromY;
	}
}
