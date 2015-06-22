#ifndef GEODATAMAPPER_H
#define GEODATAMAPPER_H

#include <QObject>
#include "geodatacreator.h"

class Grid;
class GridAttributeContainer;

class GeoDataMapperSetting
{

public:
	GeoDataMapperSetting() {}
	virtual ~GeoDataMapperSetting() {}
};

class GeoDataMapper : public QObject
{

public:
	/// Constructor
	GeoDataMapper(GeoDataCreator* parent) : QObject(parent) {
		m_grid = nullptr;
		m_container = nullptr;
		m_geodata = nullptr;
	}
	virtual ~GeoDataMapper(){}
	void setTarget(Grid* grid, GridAttributeContainer* container, GeoData* geodata) {
		m_grid = grid;
		m_container = container;
		m_geodata = geodata;
	}
	/// Initialize mapping setting
	virtual GeoDataMapperSetting* initialize(bool* boolMap) = 0;
	/// Map values to grid related condition.
	virtual void map(bool* boolMap, GeoDataMapperSetting* setting) = 0;
	/// Terminate mapping setting
	virtual void terminate(GeoDataMapperSetting* setting) = 0;
	const QString& caption() const {return m_caption;}

protected:
	GeoDataCreator* creator() const {
		return dynamic_cast<GeoDataCreator*>(parent());
	}

	QString m_caption;
	Grid* m_grid;
	GridAttributeContainer* m_container;
	GeoData* m_geodata;
};

#endif // GEODATAMAPPER_H
