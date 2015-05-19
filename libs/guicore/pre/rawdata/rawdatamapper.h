#ifndef RAWDATAMAPPER_H
#define RAWDATAMAPPER_H

#include <QObject>
#include "rawdatacreator.h"

class Grid;
class GridRelatedConditionContainer;

class RawDataMapperSetting
{
public:
	RawDataMapperSetting() {}
	virtual ~RawDataMapperSetting() {}
};

class RawDataMapper : public QObject
{
public:
	/// Constructor
	RawDataMapper(RawDataCreator* parent) : QObject(parent) {
		m_grid = nullptr;
		m_container = nullptr;
		m_rawdata = nullptr;
	}
	virtual ~RawDataMapper(){}
	void setTarget(Grid* grid, GridRelatedConditionContainer* container, RawData* rawdata) {
		m_grid = grid;
		m_container = container;
		m_rawdata = rawdata;
	}
	/// Initialize mapping setting
	virtual RawDataMapperSetting* initialize(bool* boolMap) = 0;
	/// Map values to grid related condition.
	virtual void map(bool* boolMap, RawDataMapperSetting* setting) = 0;
	/// Terminate mapping setting
	virtual void terminate(RawDataMapperSetting* setting) = 0;
	const QString& caption() const {return m_caption;}

protected:
	RawDataCreator* creator() const {
		return dynamic_cast<RawDataCreator*>(parent());
	}

	QString m_caption;
	Grid* m_grid;
	GridRelatedConditionContainer* m_container;
	RawData* m_rawdata;
};

#endif // RAWDATAMAPPER_H
