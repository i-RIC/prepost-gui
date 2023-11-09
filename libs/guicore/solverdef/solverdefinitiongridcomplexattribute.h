#ifndef SOLVERDEFINITIONGRIDRELATEDCOMPLEXCONDITION_H
#define SOLVERDEFINITIONGRIDRELATEDCOMPLEXCONDITION_H

#include "../guicore_global.h"
#include "solverdefinitiongridattributeinteger.h"

class GUICOREDLL_EXPORT SolverDefinitionGridComplexAttribute : public SolverDefinitionGridAttributeInteger
{

public:
	SolverDefinitionGridComplexAttribute(QDomElement node, SolverDefinition* solverDef, Position pos, int order);
	~SolverDefinitionGridComplexAttribute();

	const QDomElement& element() const;

	GridAttributeEditWidget* editWidget(QWidget* parent) override;
	GridAttributeVariationEditWidget* variationEditWidget(QWidget* parent) override;
	GeoData* buildBackgroundGeoData(ProjectDataItem* parent) override;

	QString undefinedString() const;
	QString undefinedEnglishString() const;

	bool isGrouped() const;

private:
	GridAttributeContainer* buildContainer(v4InputGrid* grid) override;

	class Impl;
	std::unique_ptr<Impl> impl;
};

#ifdef _DEBUG
	#include "private/solverdefinitiongridcomplexattribute_impl.h"
#endif // _DEBUG

#endif // SOLVERDEFINITIONGRIDRELATEDCOMPLEXCONDITION_H
