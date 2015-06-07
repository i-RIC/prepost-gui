#include "structured15dgridwithcrosssectioncgnsimporter.h"
#include "structured15dgridwithcrosssectionprojectimporter.h"

CgnsGridImporter* Structured15DGridWithCrossSectionProjectImporter::getCgnsImporter()
{
	return new Structured15DGridWithCrossSectionCgnsImporter();
}
