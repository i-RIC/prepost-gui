#include "geodatapointmapcellmappert.h"
#include "geodatapointmaplandxmlimporter.h"
#include "geodatapointmapnodemappert.h"
#include "geodatapointmaprealexporter.h"
#include "geodatapointmaprealimporter.h"
#include "geodatapointmaprealtextimporter.h"
#include "geodatapointmapstlexporter.h"
#include "geodatapointmapstlimporter.h"
#include "geodatapointmaptincreator.h"
#include "geodatapointmapt.h"
#include "geodatapointmaplandxmlexporter.h"
#include "geodatapointmapvtkexporter.h"
#include "geodatapointmapwebimporter.h"

#include <vtkDoubleArray.h>

GeoDataPointmapTINCreator::GeoDataPointmapTINCreator() :
	GeoDataCreator {"tin", tr("TIN")}
{
	importers().push_back(new GeoDataPointmapSTLImporter(this));
	importers().push_back(new GeoDataPointmapLandXmlImporter(this));

	exporters().push_back(new GeoDataPointmapLandXmlExporter(this));
	exporters().push_back(new GeoDataPointmapRealExporter(this));
	exporters().push_back(new GeoDataPointmapSTLExporter(this));
	exporters().push_back(new GeoDataPointmapVTKExporter(this));

	nodeMappers().push_back(new GeoDataPointmapNodeMapperT<double, vtkDoubleArray>(this));
	cellMappers().push_back(new GeoDataPointmapCellMapperT<double, vtkDoubleArray>(this));
}

GeoData* GeoDataPointmapTINCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	GeoDataPointmap* data = new GeoDataPointMapT<double, vtkDoubleArray>(parent, this, condition);
	data->setDefaultMapper();
	return data;
}

QString GeoDataPointmapTINCreator::name(unsigned int index) const
{
	return QString("pointcloud%1").arg(index);
}

QString GeoDataPointmapTINCreator::defaultCaption(unsigned int index) const
{
	return QString(tr("Point Cloud Data%1")).arg(index);
}

bool GeoDataPointmapTINCreator::isCompatibleWith(SolverDefinitionGridAttribute* condition) const
{
	if (dynamic_cast<SolverDefinitionGridAttributeT<double>* >(condition) == nullptr) {return false;}
	if (condition->dimensions().size() > 0) {return false;}
	if (condition->isReferenceInformation()) {return false;}
	return true;
}
