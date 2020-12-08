#ifndef COORDINATESYSTEM_IMPL_H
#define COORDINATESYSTEM_IMPL_H

#include "../coordinatesystem.h"

#include <QString>
#define ACCEPT_USE_OF_DEPRECATED_PROJ_API_H
#include <proj_api.h>

class CoordinateSystem::Impl
{
public:
	QString m_name;
	QString m_caption;
	QString m_searchTarget;
	bool m_initialized;

	QString m_proj4LonLatStr;
	QString m_proj4PlaneStr;
	projPJ m_proj4LonLatPj;
	projPJ m_proj4PlanePj;
};

#endif // COORDINATESYSTEM_IMPL_H

