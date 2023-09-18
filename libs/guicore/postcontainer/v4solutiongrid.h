#ifndef V4SOLUTIONGRID_H
#define V4SOLUTIONGRID_H

#include "../guicore_global.h"
#include "../solverdef/solverdefinitiongridtype.h"

#include <string>

class v4Grid;

class GUICOREDLL_EXPORT v4SolutionGrid
{
public:
	const static std::string IBC;
	const static double IBCLimit;

	enum class Position {
		Node,
		CellCenter,
		IFace,
		JFace,
		KFace
	};

	v4SolutionGrid(SolverDefinitionGridType* gridType, v4Grid* grid);
	~v4SolutionGrid();

	SolverDefinitionGridType* gridType() const;
	SolverDefinitionGridType::GridType type() const;

	v4Grid* grid() const;

	bool scalarValueExists(Position pos) const;
	bool vectorValueExists(Position pos) const;
	bool ibcExists(Position pos) const;
	std::string elevationName(Position pos) const;

private:
	class Impl;
	std::unique_ptr<Impl> impl;

	class AttributeDataProvider;
};

#endif // V4SOLUTIONGRID_H
