#include "geodatapolygongroup.h"
#include "geodatapolygongroupcreator.h"
#include "geodatapolygongroupshpimporter.h"
#include "geodatapolygongroupshpexporter.h"

GeoDataPolygonGroupCreator::GeoDataPolygonGroupCreator(const QString& typeName) :
	GeoDataCreator {typeName, tr("PolygonGroup")}
{
	importers().push_back(new GeoDataPolygonGroupShpImporter(this));
	exporters().push_back(new GeoDataPolygonGroupShpExporter(this));
}

QString GeoDataPolygonGroupCreator::name(unsigned int index) const
{
	return QString("polygonGroup%1").arg(index);
}

QString GeoDataPolygonGroupCreator::defaultCaption(unsigned int index) const
{
	return QString(tr("PolygonGroup%1")).arg(index);
}

bool GeoDataPolygonGroupCreator::isCreatable() const
{
	return true;
}

GeoData* GeoDataPolygonGroupCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	auto g = new GeoDataPolygonGroup(parent, this, condition);
	g->setPosition(condition->position());
	g->setDefaultMapper();
	return g;
}
