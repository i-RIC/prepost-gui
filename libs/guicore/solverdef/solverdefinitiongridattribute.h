#ifndef SOLVERDEFINITIONGRIDATTRIBUTE_H
#define SOLVERDEFINITIONGRIDATTRIBUTE_H

#include "../guicore_global.h"
#include "solverdefinitionnode.h"

#include <QVariant>

#include <string>
#include <vector>

class QString;
class QWidget;
class ColorTransferFunctionContainer;
class Grid;
class GridAttributeContainer;
class GridAttributeEditWidget;
class GridAttributeEditDialog;
class GridAttributeVariationEditWidget;
class GridAttributeVariationEditDialog;
class GeoData;
class LookupTableContainer;
class ProjectDataItem;
class ScalarsToColorsContainer;
class ScalarsToColorsEditDialog;
class ScalarsToColorsEditWidget;
class SolverDefinition;
class SolverDefinitionGridAttributeDimension;

class GUICOREDLL_EXPORT SolverDefinitionGridAttribute : public SolverDefinitionNode
{
public:
	enum Position {
		Node,           ///< Node (vertex)
		CellCenter      ///< Cell center
	};
	SolverDefinitionGridAttribute(const QDomElement& elem, SolverDefinition* definition, Position pos, bool isOption, int order);
	virtual ~SolverDefinitionGridAttribute();

	/// @name Properties
	//@{
	const std::string& name() const;
	const std::string& englishCaption() const;
	const QString& caption() const;
	const QVariant& variantDefaultValue() const;
	const QVariant& variantMaximumValue() const;
	const QVariant& variantMinimumValue() const;
	Position position() const;
	bool isOption() const;
	const QString& mapping() const;
	const QString& mappingArgs() const;
	int order() const;

	bool isReferenceInformation() const;
	void setIsReferenceInformation(bool ref);

	const std::vector<SolverDefinitionGridAttributeDimension*>& dimensions() const;
	std::vector<SolverDefinitionGridAttributeDimension*>& dimensions();
	//@}

	/// @name Interface building functions
	//@{
	GridAttributeContainer* container(Grid* grid);

	GridAttributeEditDialog* editDialog(QWidget* parent);
	virtual GridAttributeEditWidget* editWidget(QWidget* parent) = 0;

	GridAttributeVariationEditDialog* variationEditDialog(QWidget* parent);
	virtual GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) = 0;

	virtual GeoData* buildBackgroundGeoData(ProjectDataItem* parent) = 0;

	virtual ScalarsToColorsContainer* createScalarsToColorsContainer(ProjectDataItem* d);
	virtual ScalarsToColorsEditWidget* createScalarsToColorsEditWidget(QWidget* parent) const;
	ScalarsToColorsEditDialog* createScalarsToColorsEditDialog(QWidget* parent) const;
	//@}
protected:
	SolverDefinition* solverDefinition() const;

	void setPosition(Position pos);

	ColorTransferFunctionContainer* createColorTransferFunctionContainer(ProjectDataItem* d);
	LookupTableContainer* createLookupTableContainer(ProjectDataItem* d) const;
	ScalarsToColorsEditWidget* createColorTransferFunctionEditWidget(QWidget* parent) const;
	ScalarsToColorsEditWidget* createLookupTableEditWidget(QWidget* parent) const;

private:
	virtual GridAttributeContainer* buildContainer(Grid* grid) = 0;

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/solverdefinitiongridattribute_impl.h"
#endif // _DEBUG

#endif // SOLVERDEFINITIONGRIDATTRIBUTE_H
