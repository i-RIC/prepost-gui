#ifndef SOLVERDEFINITIONGRIDTYPE_H
#define SOLVERDEFINITIONGRIDTYPE_H

#include "../guicore_global.h"
#include "solverdefinitionnode.h"

#include <string>
#include <vector>

class ColorMapSettingEditWidgetI;
class Grid;
class SolverDefinitionGridAttribute;
class SolverDefinitionGridComplexAttribute;
class SolverDefinitionGridOutput;
class SolverDefinitionBoundaryCondition;
class SolverDefinitionTranslator;

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
	const std::vector<SolverDefinitionGridAttribute*>& gridAttributes() const;
	SolverDefinitionGridAttribute* gridAttribute(const std::string& name) const;
	const std::vector<SolverDefinitionGridComplexAttribute*>& gridComplexAttributes() const;
	SolverDefinitionGridComplexAttribute* gridComplexAttribute(const std::string& name) const;
	//@{

	/// @name Boundary conditions
	//@{
	const std::vector<SolverDefinitionBoundaryCondition*>& boundaryConditions() const;
	SolverDefinitionBoundaryCondition* boundaryCondition(const std::string& name) const;
	//@}

	/// @name Outputs
	//@{
	const std::vector<SolverDefinitionGridOutput*>& outputs() const;
	SolverDefinitionGridOutput* output(const std::string& name) const;
	QString outputCaption(const std::string& name) const;
	QString vectorOutputCaption(const std::string& name) const;
	QStringList outputCaptions(const std::vector<std::string>& names) const;
	//@}

	/// @name Properties
	//@{
	const std::vector<GridType>& availableGridTypes() const;
	GridType defaultGridType() const;
	const std::vector<QString>& availableGridGenerators() const;
	const std::string& name() const;
	const QString& caption() const;
	void setCaption(const QString& caption);

	bool isPrimary() const;
	bool multiple() const;
	bool isOptional() const;
	bool isKeepOrder() const;
	bool post() const;
	//@}

	/// @name Functions to build objects
	//@{
	void buildGridAttributes(Grid* grid) const;
	/// Returns a pointer to a grid that has no data.
	Grid* emptyGrid() const;
	Grid* createEmptyGrid();
	Grid* createEmptyGrid(GridType type);
	//@}

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
