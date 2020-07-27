#ifndef PROJECT_IMPL_H
#define PROJECT_IMPL_H

#include "../project.h"
#include "../../base/rootdataitem.h"
#include "../../baseline/baseline.h"
#include "../../crosssections/crosssections.h"
#include "../../elevationpoints/elevationpoints.h"
#include "../../watersurfaceelevationpoints/watersurfaceelevationpoints.h"

#include "../../../csbuilder/crosssectionbuildermapalltonearest.h"
#include "../../../csbuilder/crosssectionbuildertemplatemapping.h"
#include "../../../csbuilder/crosssectionbuildertin.h"

#include <QPointF>
#include <QString>

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
	CrossSectionBuilderTemplateMapping m_builderTemplate;
	CrossSectionBuilderTin m_builderTin;

	CrossSectionBuilderI* m_currentBuilder;

	QPointF m_offset;
	QString m_rivFileName;
	QString m_csvFileName;
	QString m_rivCsvFolderName;

	QString m_filename;
	QString m_tempDir;

	bool m_isModified;
};

#endif // PROJECT_IMPL_H
