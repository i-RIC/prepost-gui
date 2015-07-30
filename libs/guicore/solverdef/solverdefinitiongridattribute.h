#ifndef SOLVERDEFINITIONGRIDRELATEDCONDITION_H
#define SOLVERDEFINITIONGRIDRELATEDCONDITION_H

#include "../guicore_global.h"
#include "solverdefinitionnode.h"

#include <QVariant>
#include <QList>

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
class SolverDefinitionGridAttributeDimension;

class GUICOREDLL_EXPORT SolverDefinitionGridAttribute : public SolverDefinitionNode
{

public:
	enum Position {
		Node,           ///< Node (vertex)
		CellCenter      ///< Cell center
	};
	/// Constructor
	SolverDefinitionGridAttribute(QDomElement node, const SolverDefinitionTranslator& translator, Position pos, bool isOption);
	virtual ~SolverDefinitionGridAttribute();

	/// @name Properties
	//@{
	const QString& name() const;
	const QString& englishCaption() const;
	const QString& caption() const;
	const QVariant& variantDefaultValue() const;
	const QVariant& variantMaximumValue() const;
	const QVariant& variantMinimumValue() const;
	Position position() const;
	bool isOption() const;

	const QList<SolverDefinitionGridAttributeDimension*>& dimensions() const;
	QList<SolverDefinitionGridAttributeDimension*>& dimensions();
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
	void setPosition(Position pos);

	ColorTransferFunctionContainer* createColorTransferFunctionContainer(ProjectDataItem* d);
	LookupTableContainer* createLookupTableContainer(ProjectDataItem* d) const;
	ScalarsToColorsEditWidget* createColorTransferFunctionEditWidget(QWidget* parent) const;
	ScalarsToColorsEditWidget* createLookupTableEditWidget(QWidget* parent) const;

private:
	virtual GridAttributeContainer* buildContainer(Grid* grid) = 0;

	class Impl;
	Impl* m_impl;
};

#endif // SOLVERDEFINITIONGRIDRELATEDCONDITION_H
