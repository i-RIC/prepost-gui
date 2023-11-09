#ifndef V4INPUTGRIDIO_H
#define V4INPUTGRIDIO_H

#include "../../guicore_global.h"

class SolverDefinitionGridType;
class v4InputGrid;
class v4Grid;
class v4Structured15dGridWithCrossSection;
class v4Structured2dGrid;
class v4Unstructured2dGrid;

class QPointF;

namespace iRICLib {
	class H5CgnsBase;
	class H5CgnsZone;
} // namespace iRICLib


class GUICOREDLL_EXPORT v4InputGridIO
{
public:
	static v4InputGrid* load(const iRICLib::H5CgnsZone& zone, SolverDefinitionGridType* gridType, const QPointF& offset, int* ier);
	static v4Structured2dGrid* loadStructured2dGrid(const iRICLib::H5CgnsZone& zone, const QPointF& offset, int* ier);
	static v4Unstructured2dGrid* loadUnstructured2dGrid(const iRICLib::H5CgnsZone& zone, const QPointF& offset, int* ier);
	static v4Structured15dGridWithCrossSection* loadStructured15DGridWithCrossSection(const iRICLib::H5CgnsZone& zone, const QPointF& offset, int* ier);

	static int saveGrid(v4InputGrid* grid, iRICLib::H5CgnsBase* base, const std::string& zoneName, const QPointF& offset, iRICLib::H5CgnsZone** zone);
	static int saveStructured2dGrid(v4Structured2dGrid* grid, iRICLib::H5CgnsBase* base, const std::string& zoneName, const QPointF& offset, iRICLib::H5CgnsZone** zone);
	static int saveUnstructured2dGrid(v4Unstructured2dGrid* grid, iRICLib::H5CgnsBase* base, const std::string& zoneName, const QPointF& offset, iRICLib::H5CgnsZone** zone);
	static int saveStructured15DGridWithCrossSection(v4Structured15dGridWithCrossSection* grid, iRICLib::H5CgnsBase* base, const std::string& zoneName, const QPointF& offset, iRICLib::H5CgnsZone** zone);

private:
	v4InputGridIO();
};

#endif // V4INPUTGRIDIO_H
