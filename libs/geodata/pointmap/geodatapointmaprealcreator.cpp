#include "geodatapointmapcellmappert.h"
#include "geodatapointmaplandxmlimporter.h"
#include "geodatapointmapnodemappert.h"
#include "geodatapointmaprealcreator.h"
#include "geodatapointmaprealexporter.h"
#include "geodatapointmaprealimporter.h"
#include "geodatapointmapstlexporter.h"
#include "geodatapointmapstlimporter.h"
#include "geodatapointmapt.h"
#include "geodatapointmaplandxmlexporter.h"
#include "geodatapointmapvtkexporter.h"
#include "geodatapointmapwebimporter.h"

#include <vtkDoubleArray.h>

GeoDataPointmapRealCreator::GeoDataPointmapRealCreator() :
	GeoDataCreator {"realPointmap", tr("Points")}
{
	importers().push_back(new GeoDataPointmapRealImporter(this));
	importers().push_back(new GeoDataPointmapSTLImporter(this));
	importers().push_back(new GeoDataPointmapLandXmlImporter(this));

	webImporters().push_back(new GeoDataPointmapWebImporter(this));

	exporters().push_back(new GeoDataPointmapLandXmlExporter(this));
	exporters().push_back(new GeoDataPointmapRealExporter(this));
	exporters().push_back(new GeoDataPointmapSTLExporter(this));
	exporters().push_back(new GeoDataPointmapVTKExporter(this));

	nodeMappers().push_back(new GeoDataPointmapNodeMapperT<double, vtkDoubleArray>(this));
	cellMappers().push_back(new GeoDataPointmapCellMapperT<double, vtkDoubleArray>(this));
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
