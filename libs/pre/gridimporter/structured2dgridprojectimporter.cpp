#include "structured2dgridprojectimporter.h"
#include "structured2dgridcgnsimporter.h"

CgnsGridImporter* Structured2DGridProjectImporter::getCgnsImporter()
{
	return new Structured2DGridCgnsImporter();
}
