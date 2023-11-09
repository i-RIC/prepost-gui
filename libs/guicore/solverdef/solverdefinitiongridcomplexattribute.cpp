#include "../pre/complex/gridcomplexconditiongrouprealeditwidget.h"
#include "../pre/gridcond/complex/gridcomplexattributecontainer.h"
#include "../pre/gridcond/complex/gridcomplexattributeeditwidget.h"
#include "../pre/geodatabackground/geodatabackgroundcomplexcreator.h"
#include "../scalarstocolors/colormapenumeratefactory.h"
#include "solverdefinitiongridcomplexattribute.h"
#include "private/solverdefinitiongridcomplexattribute_impl.h"

#include <misc/xmlsupport.h>

SolverDefinitionGridComplexAttribute::Impl::Impl(const QDomElement& elem)
{
	load(elem);
}

void SolverDefinitionGridComplexAttribute::Impl::load(const QDomElement& elem)
{
	QDomElement defNode = iRIC::getChildNode(elem, "Definition").toElement();
	m_element = defNode;
	m_isGrouped = ! (defNode.attribute("grouped") == "false");
}

SolverDefinitionGridComplexAttribute::SolverDefinitionGridComplexAttribute(QDomElement node, SolverDefinition* solverDef, Position pos, int order) :
	SolverDefinitionGridAttributeInteger {node, solverDef, pos, false, order},
	impl {new Impl {node}}
{
	setColorMapFactory(new ColorMapEnumerateFactory());
	setVariantDefaultValue(QVariant(1));
}

SolverDefinitionGridComplexAttribute::~SolverDefinitionGridComplexAttribute()
{}

const QDomElement& SolverDefinitionGridComplexAttribute::element() const
{
	return impl->m_element;
}

GridAttributeEditWidget* SolverDefinitionGridComplexAttribute::editWidget(QWidget* parent)
{
	return new GridComplexAttributeEditWidget(parent, this);
}

GridAttributeVariationEditWidget* SolverDefinitionGridComplexAttribute::variationEditWidget(QWidget* parent)
{
	if (impl->m_isGrouped) {return nullptr;}

	return new GridComplexConditionGroupRealEditWidget(parent, this);
}

GeoData* SolverDefinitionGridComplexAttribute::buildBackgroundGeoData(ProjectDataItem* parent)
{
	return GeoDataBackgroundComplexCreator::instance()->create(parent, this);
}

QString SolverDefinitionGridComplexAttribute::undefinedString() const
{
	return QObject::tr("(Undefined)");
}

QString SolverDefinitionGridComplexAttribute::undefinedEnglishString() const
{
	return "(Undefined)";
}

bool SolverDefinitionGridComplexAttribute::isGrouped() const
{
	return impl->m_isGrouped;
}

GridAttributeContainer* SolverDefinitionGridComplexAttribute::buildContainer(v4InputGrid* grid)
{
	return new GridComplexAttributeContainer(grid, this);
}
