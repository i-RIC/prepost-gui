#ifndef PROJECT_IMPL_H
#define PROJECT_IMPL_H

#include "../project.h"
#include "../../base/rootdataitem.h"
#include "../../baseline/baseline.h"
#include "../../crosssections/crosssections.h"
#include "../../elevationpoints/elevationpoints.h"
#include "../../watersurfaceelevationpoints/watersurfaceelevationpoints.h"

#include "../../../csbuilder/crosssectionbuildermapalltonearest.h"
#include "../../../csbuilder/crosssectionbuildertin.h"

#include <QPointF>

class Project::Impl
{
public:
	Impl(Project* project);
	~Impl();

	RootDataItem m_rootDataItem;

	ElevationPoints m_elevationPoints;
	WaterSurfaceElevationPoints m_waterSurfaceElevationPoints;
	CrossSections m_crossSections;
	BaseLine m_baseLine;

	CrossSectionBuilderMapAllToNearest m_builderNearest;
	CrossSectionBuilderTin m_builderTin;

	CrossSectionBuilderI* m_currentBuilder;

	QPointF m_offset;
};

#endif // PROJECT_IMPL_H
