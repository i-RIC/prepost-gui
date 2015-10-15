#ifndef SOLVERDEFINITIONGRIDTYPE_H
#define SOLVERDEFINITIONGRIDTYPE_H

#include "../guicore_global.h"
#include "solverdefinitionnode.h"
#include <QList>

class SolverDefinitionGridAttribute;
class SolverDefinitionGridComplexAttribute;
class SolverDefinitionBoundaryCondition;
class Grid;

class GUICOREDLL_EXPORT SolverDefinitionGridType : public SolverDefinitionNode
{

public:
	enum GridType {
		gtUnknownGrid,
		gtNormal1DGrid,
		gtNormal1_5DGrid,
		gtNormal1_5DGridWithCrosssection,
		gtStructured2DGrid,
		gtUnstructured2DGrid
	};
	/// Constructor for dummy grid
	SolverDefinitionGridType(const QString& name, const QString& caption);
	/// Constructor
	SolverDefinitionGridType(QDomElement node, const SolverDefinitionTranslator& translator, bool isPrimary);
	/// Destructor
	~SolverDefinitionGridType();

	/// @name Grid attributes
	//@{
	const QList<SolverDefinitionGridAttribute*>& gridRelatedConditions() const;
	SolverDefinitionGridAttribute* gridRelatedCondition(const QString& name) const;
	const QList<SolverDefinitionGridComplexAttribute*>& gridRelatedComplexConditions() const;
	SolverDefinitionGridComplexAttribute* gridRelatedComplexCondition(const QString& name) const;
	//@{

	/// @name Boundary conditions
	//@{
	const QList<SolverDefinitionBoundaryCondition*>& boundaryConditions() const;
	SolverDefinitionBoundaryCondition* boundaryCondition(const QString& name) const;
	//@}

	/// @name Properties
	//@{
	const QList<GridType>& availableGridTypes() const;
	GridType defaultGridType() const;
	const QList<QString>& availableGridGenerators() const;
	const QString& name() const;
	const QString& caption() const;
	bool isPrimary() const;
	bool multiple() const;
	bool isOptional() const;
	bool isKeepOrder() const;
	void setCaption(const QString& caption);
	//@}

	/// @name Functions to build objects
	//@{
	void buildGridRelatedConditions(Grid* grid) const;
	/// Returns a pointer to a grid that has no data.
	Grid* emptyGrid() const;
	Grid* createEmptyGrid();
	Grid* createEmptyGrid(GridType type);
	//@}

	/// Returns the caption for the solution (calculation result)
	QString solutionCaption(const QString& name) const;

private:
	class Impl;
	Impl* m_impl;
};

#endif // SOLVERDEFINITIONGRIDTYPE_H
