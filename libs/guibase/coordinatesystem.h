#ifndef COORDINATESYSTEM_H
#define COORDINATESYSTEM_H

#include "guibase_global.h"

#include <QString>
#include <proj_api.h>

class GUIBASEDLL_EXPORT CoordinateSystem
{
public:
	CoordinateSystem(const QString& name, const QString& caption, const QString& lonlat, const QString& plane);
	virtual ~CoordinateSystem();

	const QString& name() const {return m_name;}
	const QString& caption() const {return m_caption;}
	const QString& searchTarget() const {return m_searchTarget;}
	void setName(const QString& name) {m_name = name;}
	void setCaption(const QString& caption);
	void init();
	void free();

	void mapGeoToGrid(double fromX, double fromY, double* toX, double* toY);
	void mapGridToGeo(double fromX, double fromY, double* toX, double* toY);

private:
	QString m_name;
	QString m_caption;
	QString m_searchTarget;
	bool m_initialized;

	QString m_proj4LonLatStr;
	QString m_proj4PlaneStr;
	projPJ m_proj4LonLatPj;
	projPJ m_proj4PlanePj;
};

#endif // COORDINATESYSTEM_H
