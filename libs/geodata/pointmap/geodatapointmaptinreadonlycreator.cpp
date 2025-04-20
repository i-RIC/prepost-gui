#include "geodatapointmapcellmapper.h"
#include "geodatapointmaplandxmlimporter.h"
#include "geodatapointmapnodemapper.h"
#include "geodatapointmaprealexporter.h"
#include "geodatapointmaprealtextimporter.h"
#include "geodatapointmapstlexporter.h"
#include "geodatapointmapstlimporter.h"
#include "geodatapointmaptinreadonlycreator.h"
#include "geodatapointmap.h"
#include "geodatapointmaplandxmlexporter.h"
#include "geodatapointmapriter3dimporter.h"
#include "geodatapointmapvtkexporter.h"
#include "geodatapointmapwebimporter.h"

#include <vtkDoubleArray.h>

GeoDataPointmapTINReadOnlyCreator::GeoDataPointmapTINReadOnlyCreator() :
	GeoDataPointmapTINCreator {"tinReadOnly"}
{}

bool GeoDataPointmapTINReadOnlyCreator::isReadOnly() const
{
	return true;
}
