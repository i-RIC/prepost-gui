#ifndef COORDINATESYSTEMBUILDER_H
#define COORDINATESYSTEMBUILDER_H

#include "guibase_global.h"

class CoordinateSystem;
#include <QList>
#include <QMap>
#include <QObject>

/// Help class that build CoordinateSystem instances
class GUIBASEDLL_EXPORT CoordinateSystemBuilder : public QObject
{
	Q_OBJECT

public:
	CoordinateSystemBuilder(QObject* parent);
	virtual ~CoordinateSystemBuilder();
	/// Returns the list of well-known coordinate systems.
	const QList<CoordinateSystem*>& systems() const {return m_systems;}
	/**
	 * @brief Get the coordinate system from name
	 * @param name the name of coordinate system
	 */
	CoordinateSystem* system(const QString& name) const;
	/**
	 * @brief Build CoordinateSystem instance from coordinate system definition in Proj.4 format
	 * @param str coordinate system definition in Proj.4 format.
	 * For example: "+proj=longlat +ellps=WGS84 +towgs84=0,0,0,0,0,0,0 +no_defs"
	 */
	CoordinateSystem* buildFromProj4String(const QString& str);

private:
	/// Setup the list of coordinate systems
	void setupMap();
	/// The list of well-known coordinate systems
	QList<CoordinateSystem*> m_systems;
	/// The dictionary from name to coordinate system
	QMap<QString, CoordinateSystem*> m_systemsMap;
};

#endif // COORDINATESYSTEMBUILDER_H
