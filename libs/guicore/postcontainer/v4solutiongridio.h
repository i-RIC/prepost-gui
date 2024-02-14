#ifndef V4SOLUTIONGRIDIO_H
#define V4SOLUTIONGRIDIO_H

#include "../guicore_global.h"

class v4Grid;
class v4Grid2d;
class v4Particles2d;
class v4Particles3d;
class v4PolyData2d;;
class v4SolutionGrid;
class v4Structured2dGrid;
class v4Structured3dGrid;
class v4Unstructured2dGrid;
class SolverDefinitionGridType;

class QPointF;

namespace iRICLib {
	class H5CgnsGridCoordinates;
	class H5CgnsZone;
} // namespace iRICLib

class v4SolutionGridIO
{
public:
	static v4SolutionGrid* loadGrid(SolverDefinitionGridType* gridType, iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier);
	static void loadGrid(v4SolutionGrid* grid, iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier);

	static v4SolutionGrid* loadParticles2d(SolverDefinitionGridType* gridType, iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier);
	static std::map<std::string, v4SolutionGrid*> loadParticleGroups2d(SolverDefinitionGridType* gridType, iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier);
	static std::map<std::string, v4SolutionGrid*> loadParticleGroupImage2d(SolverDefinitionGridType* gridType, iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier);

	static v4SolutionGrid* loadParticles3d(SolverDefinitionGridType* gridType, iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier);
	static std::map<std::string, v4SolutionGrid*> loadParticleGroups3d(SolverDefinitionGridType* gridType, iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier);

	static std::map<std::string, v4SolutionGrid*> loadPolyDataGroup2d(SolverDefinitionGridType* gridType, iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier);

private:
	static v4Structured2dGrid* loadStructured2dGrid(iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier);
	static void loadStructured2dGrid(v4Structured2dGrid* grid, iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier, bool forceLoadCoords = false);

	static v4Unstructured2dGrid* loadUnstructured2dGrid(iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier);
	static void loadUnstructured2dGrid(v4Unstructured2dGrid* grid, iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier, bool forceLoadCoords = false);

	static v4Structured3dGrid* loadStructured3dGrid(iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier);
	static void loadStructured3dGrid(v4Structured3dGrid* grid, iRICLib::H5CgnsZone* zone, const QPointF& offset, int* ier, bool forceLoadCoords = false);
	static int loadCoordinates2d(v4Grid2d* grid, iRICLib::H5CgnsZone* zone, const QPointF& offset, bool forceLoadCoords);

	v4SolutionGridIO();
};

#endif // V4SOLUTIONGRIDIO_H
