#ifndef COORDINATESYSTEM_H
#define COORDINATESYSTEM_H

#include "guibase_global.h"

class QString;

/// Coordinate system container
class GUIBASEDLL_EXPORT CoordinateSystem
{
public:
	CoordinateSystem(const QString& name, const QString& caption, const QString& lonlat, const QString& plane);
	~CoordinateSystem();

	const QString& name() const; // like "EPSG:2222"
	void setName(const QString& name);

	const QString& caption() const;
	void setCaption(const QString& caption);

	const QString& searchTarget() const;

	const QString& proj4LonLatStr() const;
	const QString& proj4PlaneStr() const;

	void init();
	void free();

	void mapGeoToGrid(double fromX, double fromY, double* toX, double* toY) const;
	void mapGridToGeo(double fromX, double fromY, double* toX, double* toY) const;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/coordinatesystem_impl.h"
#endif // _DEBUG

#endif // COORDINATESYSTEM_H
