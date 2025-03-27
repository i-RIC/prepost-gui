#include "geodatapointmapcellmapper.h"
#include "geodatapointmaplandxmlimporter.h"
#include "geodatapointmapnodemapper.h"
#include "geodatapointmaprealreadonlycreator.h"
#include "geodatapointmaprealexporter.h"
#include "geodatapointmaprealreadonlycreator.h"
#include "geodatapointmaprealtextimporter.h"
#include "geodatapointmapstlexporter.h"
#include "geodatapointmapstlimporter.h"
#include "geodatapointmap.h"
#include "geodatapointmaplandxmlexporter.h"
#include "geodatapointmapvtkexporter.h"
#include "geodatapointmapwebimporter.h"

#include <vtkDoubleArray.h>

GeoDataPointmapRealReadOnlyCreator::GeoDataPointmapRealReadOnlyCreator() :
	GeoDataCreator {"realPointmapReadOnly", tr("Point Cloud Data (read only)")}
{
	// importers().push_back(new GeoDataPointmapRealImporter(this));
	importers().push_back(new GeoDataPointmapRealTextImporter(this));

	exporters().push_back(new GeoDataPointmapLandXmlExporter(this));
	exporters().push_back(new GeoDataPointmapRealExporter(this));
	exporters().push_back(new GeoDataPointmapSTLExporter(this));
	exporters().push_back(new GeoDataPointmapVTKExporter(this));
}

GeoData* GeoDataPointmapRealReadOnlyCreator::create(ProjectDataItem* parent, SolverDefinitionGridAttribute* condition)
{
	return new GeoDataPointmap(parent, this, condition);
}

QString GeoDataPointmapRealReadOnlyCreator::name(unsigned int index) const
{
	return QString("pointcloud%1").arg(index);
}

QString GeoDataPointmapRealReadOnlyCreator::defaultCaption(unsigned int index) const
{
	return QString(tr("Point Cloud Data%1")).arg(index);
}

bool GeoDataPointmapRealReadOnlyCreator::isCompatibleWith(SolverDefinitionGridAttribute* condition) const
{
	if (dynamic_cast<SolverDefinitionGridAttributeT<double>* >(condition) == nullptr) {return false;}
	if (condition->position() == SolverDefinitionGridAttribute::Position::IFace) {return false;}
	if (condition->position() == SolverDefinitionGridAttribute::Position::JFace) {return false;}
	if (condition->dimensions().size() > 0) {return false;}
	if (condition->isReferenceInformation()) {return false;}
	return true;
}

bool GeoDataPointmapRealReadOnlyCreator::isReadOnly() const
{
	return true;
}
