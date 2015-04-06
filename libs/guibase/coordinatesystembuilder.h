#ifndef COORDINATESYSTEMBUILDER_H
#define COORDINATESYSTEMBUILDER_H

#include "guibase_global.h"

class CoordinateSystem;
#include <QList>
#include <QMap>
#include <QObject>

class GUIBASEDLL_EXPORT CoordinateSystemBuilder : public QObject
{
	Q_OBJECT
public:
	CoordinateSystemBuilder(QObject* parent);
	virtual ~CoordinateSystemBuilder();
	const QList<CoordinateSystem*>& systems() const {return m_systems;}
	CoordinateSystem* system(const QString& name) const;
	CoordinateSystem* buildFromProj4String(const QString& str);

private:
	void setupMap();

	QList<CoordinateSystem*> m_systems;
	QMap<QString, CoordinateSystem*> m_systemsMap;
};

#endif // COORDINATESYSTEMBUILDER_H
