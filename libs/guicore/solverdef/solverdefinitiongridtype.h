#ifndef SOLVERDEFINITIONGRIDTYPE_H
#define SOLVERDEFINITIONGRIDTYPE_H

#include "../guicore_global.h"
#include "solverdefinitionnode.h"
#include <QList>

#include <string>
#include <vector>

class ColorMapSettingEditWidgetI;
class Grid;
class SolverDefinitionGridAttribute;
class SolverDefinitionGridComplexAttribute;
class SolverDefinitionBoundaryCondition;
class SolverDefinitionTranslator;
class SolverDefinitionOutput;

class QStringList;
class QWidget;

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
	SolverDefinitionGridType(const std::string& name, const QString& caption);
	SolverDefinitionGridType(QDomElement node, SolverDefinition* solverDef, bool isPrimary);
	~SolverDefinitionGridType();

	/// @name Grid attributes
	//@{
	const QList<SolverDefinitionGridAttribute*>& gridAttributes() const;
	SolverDefinitionGridAttribute* gridAttribute(const std::string& name) const;
	const QList<SolverDefinitionGridComplexAttribute*>& gridComplexAttributes() const;
	SolverDefinitionGridComplexAttribute* gridComplexAttribute(const std::string& name) const;
	//@{

	/// @name Boundary conditions
	//@{
	const QList<SolverDefinitionBoundaryCondition*>& boundaryConditions() const;
	SolverDefinitionBoundaryCondition* boundaryCondition(const std::string& name) const;
	//@}

	/// @name Properties
	//@{
	const QList<GridType>& availableGridTypes() const;
	GridType defaultGridType() const;
	const QList<QString>& availableGridGenerators() const;
	const std::string& name() const;
	const QString& caption() const;
	bool isPrimary() const;
	bool multiple() const;
	bool isOptional() const;
	bool isKeepOrder() const;
	void setCaption(const QString& caption);
	//@}

	/// @name Functions to build objects
	//@{
	void buildGridAttributes(Grid* grid) const;
	/// Returns a pointer to a grid that has no data.
	Grid* emptyGrid() const;
	Grid* createEmptyGrid();
	Grid* createEmptyGrid(GridType type);
	//@}

	/// Returns the caption for the solution (calculation result)
	QString solutionCaption(const std::string& name) const;
	QStringList solutionCaptions(const std::vector<std::string>& names) const;
	SolverDefinitionOutput* output(const std::string& name);

	QString gridAttributeCaption(const std::string& name) const;
	ColorMapSettingEditWidgetI* createGridAttributeColorMapSettingEditWidget(const std::string& name, QWidget* parent);

private:
	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/solverdefinitiongridtype_impl.h"
#endif // _DEBUG

#endif // SOLVERDEFINITIONGRIDTYPE_H
