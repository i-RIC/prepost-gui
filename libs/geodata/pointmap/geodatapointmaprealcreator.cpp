#include "geodatapointmapcellmappert.h"
#include "geodatapointmapnodemappert.h"
#include "geodatapointmaprealcreator.h"
#include "geodatapointmaprealexporter.h"
#include "geodatapointmaprealimporter.h"
#include "geodatapointmapstlexporter.h"
#include "geodatapointmapstlimporter.h"
#include "geodatapointmapt.h"
#include "geodatapointmapvtkexporter.h"
#include "geodatapointmapwebimporter.h"

#include <vtkDoubleArray.h>

GeoDataPointmapRealCreator::GeoDataPointmapRealCreator() :
	GeoDataCreator {"realPointmap", tr("Points")}
{
	importers().append(new GeoDataPointmapRealImporter(this));
	importers().append(new GeoDataPointmapSTLImporter(this));

	webImporters().append(new GeoDataPointmapWebImporter(this));

	exporters().append(new GeoDataPointmapRealExporter(this));
	exporters().append(new GeoDataPointmapSTLExporter(this));
	exporters().append(new GeoDataPointmapVTKExporter(this));

	nodeMappers().append(new GeoDataPointmapNodeMapperT<double, vtkDoubleArray>(this));
	cellMappers().append(new GeoDataPointmapCellMapperT<double, vtkDoubleArray>(this));
}

GeoData* GeoDataPointmapRealCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	GeoDataPointmap* data = new GeoDataPointMapT<double, vtkDoubleArray>(parent, this, condition);
	data->setDefaultMapper();
	return data;
}

QString GeoDataPointmapRealCreator::name(unsigned int index) const
{
	return QString("pointset%1").arg(index);
}

QString GeoDataPointmapRealCreator::defaultCaption(unsigned int index) const
{
	return QString(tr("Points%1")).arg(index);
}

bool GeoDataPointmapRealCreator::isCompatibleWith(SolverDefinitionGridAttribute* condition) const
{
	if (dynamic_cast<SolverDefinitionGridAttributeT<double>* >(condition) == nullptr) {return false;}
	if (condition->dimensions().size() > 0) {return false;}
	return true;
}
