#ifndef SOLVERDEFINITIONGRIDATTRIBUTE_H
#define SOLVERDEFINITIONGRIDATTRIBUTE_H

#include "../guicore_global.h"
#include "solverdefinitionnode.h"

#include <QVariant>

#include <string>
#include <vector>

class ColorMapEngineI;
class ColorMapFactoryI;
class ColorMapLegendSettingContainerI;
class ColorMapSettingContainerI;
class ColorMapSettingEditDialog;
class ColorMapSettingEditWidgetI;
class ColorMapSettingToolBarWidget;
class ColorTransferFunctionContainer;
class v4InputGrid;
class GridAttributeContainer;
class GridAttributeEditDialog;
class GridAttributeEditNameAndValueDialog;
class GridAttributeEditWidget;
class GridAttributeStringConverter;
class GridAttributeVariationEditWidget;
class GridAttributeVariationEditDialog;
class GeoData;
class ProjectDataItem;
class SolverDefinition;
class SolverDefinitionGridAttributeDimension;

class QString;
class QWidget;

class GUICOREDLL_EXPORT SolverDefinitionGridAttribute : public SolverDefinitionNode
{
public:
	enum class Position {
		Node,           ///< Node (vertex)
		CellCenter,     ///< Cell center
		IFace,          ///< I-direction Edge
		JFace           ///< J-direction Edge
	};
	SolverDefinitionGridAttribute();
	SolverDefinitionGridAttribute(const QDomElement& elem, SolverDefinition* definition, Position pos, bool isOption, int order);
	virtual ~SolverDefinitionGridAttribute();

	/// @name Properties
	//@{
	const std::string& name() const;
	const std::string& englishCaption() const;
	const QString& caption() const;
	void setCaption(const QString& cap);
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
	GridAttributeContainer* container(v4InputGrid* grid);

	virtual GridAttributeStringConverter* stringConverter() const = 0;

	GridAttributeEditDialog* editDialog(QWidget* parent);
	GridAttributeEditNameAndValueDialog* editNameAndValueDialog(QWidget* parent);
	virtual GridAttributeEditWidget* editWidget(QWidget* parent) = 0;

	GridAttributeVariationEditDialog* variationEditDialog(QWidget* parent);
	virtual GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) = 0;

	virtual GeoData* buildBackgroundGeoData(ProjectDataItem* parent) = 0;
	virtual QVariant colorMapValue(const QVariant& value) const;

	ColorMapSettingContainerI* createColorMapSettingContainer() const;
	ColorMapLegendSettingContainerI* createColorMapLegendSettingContainer() const;
	ColorMapSettingEditWidgetI* createColorMapSettingEditWidget(QWidget* parent) const;
	ColorMapSettingEditDialog* createColorMapSettingEditDialog(QWidget* parent) const;
	ColorMapSettingToolBarWidget* createColorMapSettingToolbarWidget(QWidget* parent) const;

	//@}
protected:
	void setPosition(Position pos);
	void setColorMapFactory(ColorMapFactoryI* factory);
	void setVariantDefaultValue(const QVariant& v);

private:
	virtual void setupColorMapSettingContainer(ColorMapSettingContainerI* c) const;
	virtual GridAttributeContainer* buildContainer(v4InputGrid* grid) = 0;

	class Impl;
	std::unique_ptr<Impl> impl;
};

#ifdef _DEBUG
	#include "private/solverdefinitiongridattribute_impl.h"
#endif // _DEBUG

#endif // SOLVERDEFINITIONGRIDATTRIBUTE_H
