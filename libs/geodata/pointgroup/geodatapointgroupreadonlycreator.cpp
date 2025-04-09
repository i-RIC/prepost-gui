#include "geodatapointgroupreadonlycreator.h"
#include "geodatapointgroupcsvexporter.h"
#include "geodatapointgroupcsvimporter.h"
#include "geodatapointgroupshpexporter.h"
#include "geodatapointgroupshpimporter.h"

#include <shapefil.h>

GeoDataPointGroupReadOnlyCreator::GeoDataPointGroupReadOnlyCreator(const QString& typeName) :
	GeoDataPolyDataGroupCreator {typeName, tr("Points")}
{
	importers().push_back(new GeoDataPointGroupShpImporter(this));
	importers().push_back(new GeoDataPointGroupCsvImporter(this));
	exporters().push_back(new GeoDataPointGroupShpExporter(this));
	exporters().push_back(new GeoDataPointGroupCsvExporter(this));
}

QString GeoDataPointGroupReadOnlyCreator::name(unsigned int index) const
{
	return QString("pointGroup%1").arg(index);
}

QString GeoDataPointGroupReadOnlyCreator::defaultCaption(unsigned int index) const
{
	return QString(tr("Points%1")).arg(index);
}

bool GeoDataPointGroupReadOnlyCreator::isCreatable() const
{
	return false;
}

int GeoDataPointGroupReadOnlyCreator::shapeType() const
{
	return SHPT_POINT;
}

QString GeoDataPointGroupReadOnlyCreator::shapeName() const
{
	return tr("point");
}

QString GeoDataPointGroupReadOnlyCreator::shapeNameCamelCase() const
{
	return tr("Point");
}

bool GeoDataPointGroupReadOnlyCreator::isReadOnly() const
{
	return true;
}
