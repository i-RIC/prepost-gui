#include "geodatapointmapcellmapper.h"
#include "geodatapointmaplandxmlimporter.h"
#include "geodatapointmapnodemapper.h"
#include "geodatapointmaprealcreator.h"
#include "geodatapointmaprealexporter.h"
#include "geodatapointmaprealimporter.h"
#include "geodatapointmaprealtextimporter.h"
#include "geodatapointmapstlexporter.h"
#include "geodatapointmapstlimporter.h"
#include "geodatapointmap.h"
#include "geodatapointmaplandxmlexporter.h"
#include "geodatapointmapvtkexporter.h"
#include "geodatapointmapwebimporter.h"

#include <vtkDoubleArray.h>

GeoDataPointmapRealCreator::GeoDataPointmapRealCreator() :
	GeoDataCreator {"realPointmap", tr("Point Cloud Data")}
{
	// importers().push_back(new GeoDataPointmapRealImporter(this));
	importers().push_back(new GeoDataPointmapRealTextImporter(this));

	webImporters().push_back(new GeoDataPointmapWebImporter(this));

	exporters().push_back(new GeoDataPointmapLandXmlExporter(this));
	exporters().push_back(new GeoDataPointmapRealExporter(this));
	exporters().push_back(new GeoDataPointmapSTLExporter(this));
	exporters().push_back(new GeoDataPointmapVTKExporter(this));
}

GeoData* GeoDataPointmapRealCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	return new GeoDataPointmap(parent, this, condition);
}

QString GeoDataPointmapRealCreator::name(unsigned int index) const
{
	return QString("pointcloud%1").arg(index);
}

QString GeoDataPointmapRealCreator::defaultCaption(unsigned int index) const
{
	return QString(tr("Point Cloud Data%1")).arg(index);
}

bool GeoDataPointmapRealCreator::isCompatibleWith(SolverDefinitionGridAttribute* condition) const
{
	if (dynamic_cast<SolverDefinitionGridAttributeT<double>* >(condition) == nullptr) {return false;}
	if (condition->position() == SolverDefinitionGridAttribute::Position::IFace) {return false;}
	if (condition->position() == SolverDefinitionGridAttribute::Position::JFace) {return false;}
	if (condition->dimensions().size() > 0) {return false;}
	if (condition->isReferenceInformation()) {return false;}
	return true;
}
