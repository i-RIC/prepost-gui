#ifndef UNSTRUCTURED2DGRIDCGNSIMPORTER_H
#define UNSTRUCTURED2DGRIDCGNSIMPORTER_H

#include "cgnsgridimporter.h"

class Unstructured2DGridCgnsImporter : public CgnsGridImporter
{
public:
	Unstructured2DGridCgnsImporter() : CgnsGridImporter(){}
	bool isGridTypeSupported(SolverDefinitionGridType::GridType gt){
		return gt == SolverDefinitionGridType::gtUnstructured2DGrid;
	}

protected:
	bool isZoneCompatible(int fn, int B, int Z);
};

#endif // UNSTRUCTURED2DGRIDCGNSIMPORTER_H
