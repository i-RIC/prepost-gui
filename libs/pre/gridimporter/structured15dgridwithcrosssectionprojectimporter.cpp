#include "structured15dgridwithcrosssectionprojectimporter.h"
#include "structured15dgridwithcrosssectioncgnsimporter.h"

CgnsGridImporter* Structured15DGridWithCrossSectionProjectImporter::getCgnsImporter()
{
	return new Structured15DGridWithCrossSectionCgnsImporter();
}
