#include "geodatapointmapcellmappert.h"
#include "geodatapointmapnodemappert.h"
#include "geodatapointmaprealcreator.h"
#include "geodatapointmaprealexporter.h"
#include "geodatapointmaprealimporter.h"
#include "geodatapointmapstlexporter.h"
#include "geodatapointmapstlimporter.h"
#include "geodatapointmapt.h"
#include "geodatapointmapvtkexporter.h"

#include <vtkDoubleArray.h>

GeoDataPointmapRealCreator::GeoDataPointmapRealCreator()
	: GeoDataCreator()
{
	m_caption = tr("Points");
	m_typeName = "realPointmap";

	m_importers.append(new GeoDataPointmapRealImporter(this));
	m_importers.append(new GeoDataPointmapSTLImporter(this));

	m_exporters.append(new GeoDataPointmapRealExporter(this));
	m_exporters.append(new GeoDataPointmapSTLExporter(this));
	m_exporters.append(new GeoDataPointmapVTKExporter(this));

	m_nodeMappers.append(new GeoDataPointmapNodeMapperT<double, vtkDoubleArray>(this));
	m_cellMappers.append(new GeoDataPointmapCellMapperT<double, vtkDoubleArray>(this));
}

GeoData* GeoDataPointmapRealCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	GeoDataPointmap* data = new GeoDataPointMapT<double, vtkDoubleArray>(parent, this, condition);
	data->setDefaultMapper();
	return data;
}

QString GeoDataPointmapRealCreator::name(unsigned int index)
{
	return QString("pointset%1").arg(index);
}

QString GeoDataPointmapRealCreator::defaultCaption(unsigned int index)
{
	return QString(tr("Points%1")).arg(index);
}

bool GeoDataPointmapRealCreator::isCompatibleWith(SolverDefinitionGridAttribute* condition)
{
	if (dynamic_cast<SolverDefinitionGridAttributeT<double>* >(condition) == nullptr) {return false;}
	if (condition->dimensions().size() > 0) {return false;}
	return true;
}
