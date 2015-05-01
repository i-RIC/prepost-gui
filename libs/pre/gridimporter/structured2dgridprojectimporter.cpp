#include "structured2dgridcgnsimporter.h"
#include "structured2dgridprojectimporter.h"

CgnsGridImporter* Structured2DGridProjectImporter::getCgnsImporter()
{
	return new Structured2DGridCgnsImporter();
}
