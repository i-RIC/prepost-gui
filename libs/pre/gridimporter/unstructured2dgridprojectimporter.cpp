#include "unstructured2dgridprojectimporter.h"
#include "unstructured2dgridcgnsimporter.h"

CgnsGridImporter* Unstructured2DGridProjectImporter::getCgnsImporter()
{
	return new Unstructured2DGridCgnsImporter();
}
