#ifndef GEODATAMAPPER_H
#define GEODATAMAPPER_H

#include "../../guicore_global.h"

#include "geodatacreator.h"

#include <QObject>

class GeoData;
class Grid;
class GridAttributeContainer;
class GeoDataMapperSettingI;

class GUICOREDLL_EXPORT GeoDataMapper : public QObject
{
public:
	GeoDataMapper(const QString& caption, GeoDataCreator* parent);
	virtual ~GeoDataMapper();

	const QString& caption() const;

	void setTarget(Grid* grid, GridAttributeContainer* container, GeoData* geodata);

	virtual GeoDataMapperSettingI* initialize(bool* boolMap) = 0;
	virtual void map(bool* boolMap, GeoDataMapperSettingI* setting) = 0;
	virtual void terminate(GeoDataMapperSettingI* setting) = 0;

protected:
	GeoDataCreator* creator() const;
	Grid* grid() const;
	GridAttributeContainer* container() const;
	GeoData* geoData() const;

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/geodatamapper_impl.h"
#endif // _DEBUG

#endif // GEODATAMAPPER_H
