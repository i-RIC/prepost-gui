#include "../pre/gridcond/complex/gridcomplexattributecontainer.h"
#include "../pre/gridcond/complex/gridcomplexattributeeditwidget.h"
#include "../pre/geodatabackground/geodatabackgroundcomplexcreator.h"
#include "../scalarstocolors/colortransferfunctioncontainer.h"
#include "solverdefinitiongridcomplexattribute.h"
#include "private/solverdefinitiongridcomplexattribute_impl.h"

#include <misc/xmlsupport.h>

SolverDefinitionGridComplexAttribute::Impl::Impl(const QDomElement& elem, SolverDefinitionGridComplexAttribute* parent)
{
	load(elem, parent);
}

void SolverDefinitionGridComplexAttribute::Impl::load(const QDomElement& elem, SolverDefinitionGridComplexAttribute* parent)
{
	QDomElement defNode = iRIC::getChildNode(elem, "Definition").toElement();
	if (defNode.attribute("position") == "cell") {
		parent->setPosition(CellCenter);
	}
	m_element = defNode;
	m_isGrouped = ! (defNode.attribute("grouped") == "false");
}

SolverDefinitionGridComplexAttribute::SolverDefinitionGridComplexAttribute(QDomElement node, const SolverDefinitionTranslator& translator, int order) :
	SolverDefinitionGridAttributeInteger {node, translator, Node, false, order},
	impl {new Impl {node, this}}
{}

SolverDefinitionGridComplexAttribute::~SolverDefinitionGridComplexAttribute()
{
	delete impl;
}

const QDomElement& SolverDefinitionGridComplexAttribute::element() const
{
	return impl->m_element;
}

GridAttributeEditWidget* SolverDefinitionGridComplexAttribute::editWidget(QWidget* parent)
{
	return new GridComplexAttributeEditWidget(parent, this);
}

GridAttributeVariationEditWidget* SolverDefinitionGridComplexAttribute::variationEditWidget(QWidget*)
{
	return nullptr;
}

GeoData* SolverDefinitionGridComplexAttribute::buildBackgroundGeoData(ProjectDataItem* parent)
{
	return GeoDataBackgroundComplexCreator::instance()->create(parent, this);
}

ScalarsToColorsContainer* SolverDefinitionGridComplexAttribute::createScalarsToColorsContainer(ProjectDataItem* d)
{
	return createColorTransferFunctionContainer(d);
}

ScalarsToColorsEditWidget* SolverDefinitionGridComplexAttribute::createScalarsToColorsEditWidget(QWidget* parent) const
{
	return createColorTransferFunctionEditWidget(parent);
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

GridAttributeContainer* SolverDefinitionGridComplexAttribute::buildContainer(Grid* grid)
{
	return new GridComplexAttributeContainer(grid, this);
}
