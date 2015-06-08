#include "unstructured2dgridcgnsimporter.h"
#include "unstructured2dgridprojectimporter.h"

CgnsGridImporter* Unstructured2DGridProjectImporter::getCgnsImporter()
{
	return new Unstructured2DGridCgnsImporter();
}
