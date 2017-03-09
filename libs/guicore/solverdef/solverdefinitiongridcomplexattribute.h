#ifndef SOLVERDEFINITIONGRIDRELATEDCOMPLEXCONDITION_H
#define SOLVERDEFINITIONGRIDRELATEDCOMPLEXCONDITION_H

#include "../guicore_global.h"
#include "solverdefinitiongridattributeinteger.h"

class GUICOREDLL_EXPORT SolverDefinitionGridComplexAttribute : public SolverDefinitionGridAttributeInteger
{

public:
	SolverDefinitionGridComplexAttribute(QDomElement node, SolverDefinition* solverDef, int order);
	~SolverDefinitionGridComplexAttribute();

	const QDomElement& element() const;

	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* /*parent*/) override;
	GeoData* buildBackgroundGeoData(ProjectDataItem* parent) override;

	ScalarsToColorsContainer* createScalarsToColorsContainer(ProjectDataItem* d) override;
	ScalarsToColorsEditWidget* createScalarsToColorsEditWidget(QWidget* parent) const override;

	QString undefinedString() const;
	QString undefinedEnglishString() const;

	bool isGrouped() const;

private:
	GridAttributeContainer* buildContainer(Grid* grid) override;

	class Impl;
	Impl* impl;
};

#ifdef _DEBUG
	#include "private/solverdefinitiongridcomplexattribute_impl.h"
#endif // _DEBUG

#endif // SOLVERDEFINITIONGRIDRELATEDCOMPLEXCONDITION_H
