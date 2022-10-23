#include "../pre/grid/grid.h"
#include "../pre/grid/structured15dgridwithcrosssection.h"
#include "../pre/grid/structured2dgrid.h"
#include "../pre/grid/unstructured2dgrid.h"
#include "../project/inputcond/inputconditionwidget.h"
#include "solverdefinition.h"
#include "solverdefinitionboundarycondition.h"
#include "solverdefinitiongridattribute.h"
#include "solverdefinitiongridattributeintegercell.h"
#include "solverdefinitiongridattributeintegernode.h"
#include "solverdefinitiongridattributeintegeroptioncell.h"
#include "solverdefinitiongridattributeintegeroptionnode.h"
#include "solverdefinitiongridattributerealcell.h"
#include "solverdefinitiongridattributerealnode.h"
#include "solverdefinitiongridattributerealoptioncell.h"
#include "solverdefinitiongridattributerealoptionnode.h"
#include "solverdefinitiongridcomplexattribute.h"
#include "solverdefinitiongridtype.h"
#include "solverdefinitionoutput.h"
#include "private/solverdefinitiongridtype_impl.h"

#include <QStringList>

#include <misc/stringtool.h>
#include <misc/xmlsupport.h>

// namespace for local functions
namespace {

SolverDefinitionGridType::GridType stringToGridType(const QString& name)
{
	if (name == "1d") {
		return SolverDefinitionGridType::gtNormal1DGrid;
	} else if (name == "1.5d") {
		return SolverDefinitionGridType::gtNormal1_5DGrid;
	} else if (name == "1.5d_withcrosssection") {
		return SolverDefinitionGridType::gtNormal1_5DGridWithCrosssection;
	} else if (name == "structured2d") {
		return SolverDefinitionGridType::gtStructured2DGrid;
	} else if (name == "unstructured2d") {
		return SolverDefinitionGridType::gtUnstructured2DGrid;
	} else {
		return SolverDefinitionGridType::gtUnknownGrid;
	}
}

} // namespace

SolverDefinitionGridType::Impl::Impl(const std::string& name, const QString& caption, SolverDefinitionGridType* parent) :
	m_name (name),
	m_caption {caption},
	m_parent {parent}
{}

SolverDefinitionGridType::Impl::Impl(const QDomElement& elem, SolverDefinition *solverDef, bool isPrimary, SolverDefinitionGridType* parent) :
	m_isPrimary {isPrimary},
	m_parent {parent}
{
	load(elem, solverDef);
}

SolverDefinitionGridType::Impl::~Impl()
{
	for (auto c : m_gridAttributes) {
		delete c;
	}
	for (const auto& pair : m_outputs) {
		delete pair.second;
	}
	delete m_emptyGrid;
}

void SolverDefinitionGridType::Impl::load(const QDomElement& elem, SolverDefinition* solverDef)
{
	auto translator = solverDef->buildTranslator();
	// set basic informations.
	m_name = iRIC::toStr(elem.attribute("name", "default"));
	m_caption = translator.translate(elem.attribute("caption", QObject::tr("Default")));
	QString multistr = elem.attribute("multiple", "false");
	m_multiple = (multistr == "true");
	QString optstr = elem.attribute("optional", "false");
	m_isOptional = (optstr == "true");
	// set grid type
	setGridType(elem);
	// set grid generators
	setGridGenerators(elem);
	// setup grid related conditions
	QDomElement grcElem = iRIC::getChildNode(elem, "GridRelatedCondition").toElement();
	setupGridAttributes(grcElem, solverDef);
	// setup boundary conditions;
	setupBoundaryConditions(elem, solverDef);
	m_emptyGrid = createEmptyGrid();
}

void SolverDefinitionGridType::Impl::setGridType(const QDomElement& elem)
{
	QString gtype = elem.attribute("gridtype", "structured2d");
	QStringList types = gtype.split(",");
	for (int i = 0; i < types.size(); ++i) {
		GridType gt = stringToGridType(types.at(i).trimmed());
		m_availableGridTypes.append(gt);
	}
	if (m_availableGridTypes.size() > 0) {
		m_defaultGridType = m_availableGridTypes.at(0);
	}
}

void SolverDefinitionGridType::Impl::setGridGenerators(const QDomElement& elem)
{
	QString gens = elem.attribute("gridgenerators", "");
	QStringList genList = gens.split(",");
	m_availableGridGenerators.clear();
	for (int i = 0; i < genList.size(); ++i) {
		QString gen = genList.at(i).trimmed();
		if (gen.isEmpty()) {continue;}
		m_availableGridGenerators.append(gen);
	}
}

void SolverDefinitionGridType::Impl::setupGridAttributes(const QDomElement& elem, SolverDefinition *solverDef)
{
	m_isKeepOrder = (elem.attribute("keepOrder") == "true");
	auto childNodes = elem.childNodes();
	int order = 1;
	for (int i = 0; i < childNodes.size(); ++i) {
		auto itemNode = childNodes.at(i);
		if (itemNode.nodeName() != "Item") {continue;}

		QDomNode defNode = iRIC::getChildNode(itemNode, "Definition");
		QDomElement itemElem = itemNode.toElement();
		QDomElement defElem = defNode.toElement();

		if (defElem.attribute("valueType") == "complex") {
			// Complex condition
			SolverDefinitionGridComplexAttribute* c = new SolverDefinitionGridComplexAttribute(itemElem, solverDef, order);
			m_gridComplexAttributes.append(c);
			m_gridComplexAttributeNameMap.insert(c->name(), c);
			++ order;
		} else {
			SolverDefinitionGridAttribute* c = nullptr;
			QString pos = defElem.attribute("position");
			if (defElem.attribute("position") == "cell") {
				if (defElem.attribute("valueType") == "integer") {
					if (InputConditionWidget::hasEnums(defElem)) {
						c = new SolverDefinitionGridAttributeIntegerOptionCell(itemElem, solverDef, order);
					} else {
						c = new SolverDefinitionGridAttributeIntegerCell(itemElem, solverDef, order);
					}
				} else if (defElem.attribute("valueType") == "real") {
					if (InputConditionWidget::hasEnums(defElem)) {
						c = new SolverDefinitionGridAttributeRealOptionCell(itemElem, solverDef, order);
					} else {
						c = new SolverDefinitionGridAttributeRealCell(itemElem, solverDef, order);
					}
				}
			} else if (defElem.attribute("position") == "node") {
				if (defElem.attribute("valueType") == "integer") {
					if (InputConditionWidget::hasEnums(defElem)) {
						c = new SolverDefinitionGridAttributeIntegerOptionNode(itemElem, solverDef, order);
					} else {
						c = new SolverDefinitionGridAttributeIntegerNode(itemElem, solverDef, order);
					}
				} else if (defElem.attribute("valueType") == "real") {
					if (InputConditionWidget::hasEnums(defElem)) {
						c = new SolverDefinitionGridAttributeRealOptionNode(itemElem, solverDef, order);
					} else {
						c = new SolverDefinitionGridAttributeRealNode(itemElem, solverDef, order);
					}
				}
			}
			if (c != nullptr) {
				m_gridAttributes.append(c);
				m_gridAttributeNameMap.insert(c->name(), c);
				++ order;
			}
		}
		itemNode = itemNode.nextSibling();
	}
}

void SolverDefinitionGridType::Impl::setupBoundaryConditions(const QDomElement& elem, SolverDefinition *solverDef)
{
	QDomNode itemNode = elem.firstChild();
	while (! itemNode.isNull()) {
		if (itemNode.nodeName() != "BoundaryCondition") {
			itemNode = itemNode.nextSibling();
			continue;
		}
		auto b = new SolverDefinitionBoundaryCondition(itemNode.toElement(), solverDef);
		m_boundaryConditions.append(b);
		m_boundaryConditionNameMap.insert(b->name(), b);
		itemNode = itemNode.nextSibling();
	}
}

void SolverDefinitionGridType::Impl::buildGridAttributes(Grid* grid) const
{
	for (auto cond : m_gridAttributes) {
		grid->addGridAttribute(cond->container(grid));
	}
	for (auto cond : m_gridComplexAttributes) {
		grid->addGridAttribute(cond->container(grid));
	}
}

Grid* SolverDefinitionGridType::Impl::createEmptyGrid()
{
	Grid* ret = nullptr;
	switch (m_defaultGridType) {
	case gtNormal1DGrid:
		// @todo not implemented yet.
		break;
	case gtNormal1_5DGrid:
		// @todo not implemented yet.
		break;
	case gtNormal1_5DGridWithCrosssection:
		ret = new Structured15DGridWithCrossSection(nullptr);
		break;
	case gtStructured2DGrid:
		ret = new Structured2DGrid(nullptr);
		break;
	case gtUnstructured2DGrid:
		ret = new Unstructured2DGrid(nullptr);
		break;
	case gtUnknownGrid:
		break;
	}
	if (ret != nullptr) {
		buildGridAttributes(ret);
	}
	return ret;
}

// Public interfaces

SolverDefinitionGridType::SolverDefinitionGridType(const std::string& name, const QString& caption) :
	SolverDefinitionNode {},
	impl {new Impl{name, caption, this}}
{}

SolverDefinitionGridType::SolverDefinitionGridType(QDomElement node, SolverDefinition* solverDef, bool isPrimary) :
	SolverDefinitionNode {node, solverDef},
	impl {new Impl {node, solverDef, isPrimary, this}}
{}

SolverDefinitionGridType::~SolverDefinitionGridType()
{
	delete impl;
}

const QList<SolverDefinitionGridAttribute*>& SolverDefinitionGridType::gridAttributes() const
{
	return impl->m_gridAttributes;
}

SolverDefinitionGridAttribute* SolverDefinitionGridType::gridAttribute(const std::string& name) const
{
	return impl->m_gridAttributeNameMap.value(name);
}

const QList<SolverDefinitionGridComplexAttribute*>& SolverDefinitionGridType::gridComplexAttributes() const
{
	return impl->m_gridComplexAttributes;
}

SolverDefinitionGridComplexAttribute* SolverDefinitionGridType::gridComplexAttribute(const std::string& name) const
{
	return impl->m_gridComplexAttributeNameMap.value(name);
}

const QList<SolverDefinitionBoundaryCondition*>& SolverDefinitionGridType::boundaryConditions() const
{
	return impl->m_boundaryConditions;
}

SolverDefinitionBoundaryCondition* SolverDefinitionGridType::boundaryCondition(const std::string& name) const
{
	return impl->m_boundaryConditionNameMap.value(name);
}

const QList<SolverDefinitionGridType::GridType>& SolverDefinitionGridType::availableGridTypes() const
{
	return impl->m_availableGridTypes;
}

SolverDefinitionGridType::GridType SolverDefinitionGridType::defaultGridType() const
{
	return impl->m_defaultGridType;
}

const QList<QString>& SolverDefinitionGridType::availableGridGenerators() const
{
	return impl->m_availableGridGenerators;
}

const std::string& SolverDefinitionGridType::name() const
{
	return impl->m_name;
}

const QString& SolverDefinitionGridType::caption() const
{
	return impl->m_caption;
}

void SolverDefinitionGridType::setCaption(const QString& caption)
{
	impl->m_caption = caption;
}

bool SolverDefinitionGridType::isPrimary() const
{
	return impl->m_isPrimary;
}

bool SolverDefinitionGridType::multiple() const
{
	return impl->m_multiple;
}

bool SolverDefinitionGridType::isOptional() const
{
	return impl->m_isOptional;
}

bool SolverDefinitionGridType::isKeepOrder() const
{
	return impl->m_isKeepOrder;
}

void SolverDefinitionGridType::buildGridAttributes(Grid* grid) const
{
	impl->buildGridAttributes(grid);
}

Grid* SolverDefinitionGridType::emptyGrid() const
{
	return impl->m_emptyGrid;
}

Grid* SolverDefinitionGridType::createEmptyGrid()
{
	return createEmptyGrid(impl->m_defaultGridType);
}

Grid* SolverDefinitionGridType::createEmptyGrid(GridType type)
{
	Grid* ret = 0;
	switch (type)
	{
	case gtNormal1DGrid:
		// @todo not implemented yet.
		break;
	case gtNormal1_5DGrid:
		// @todo not implemented yet.
		break;
	case gtNormal1_5DGridWithCrosssection:
		ret = new Structured15DGridWithCrossSection(0);
		break;
	case gtStructured2DGrid:
		ret = new Structured2DGrid(0);
		break;
	case gtUnstructured2DGrid:
		ret = new Unstructured2DGrid(0);
		break;
	case gtUnknownGrid:
		break;
	}
	if (ret != 0){
		buildGridAttributes(ret);
	}
	return ret;
}

QString SolverDefinitionGridType::solutionCaption(const std::string& name) const
{
	return name.c_str();
}

QStringList SolverDefinitionGridType::solutionCaptions(const std::vector<std::string>& names) const
{
	QStringList ret;
	for (auto name : names) {
		ret.push_back(solutionCaption(name));
	}
	return ret;
}

SolverDefinitionOutput* SolverDefinitionGridType::output(const std::string& name)
{
	auto it = impl->m_outputs.find(name);
	if (it != impl->m_outputs.end()) {
		return it->second;
	}

	// @todo it should be read when initializing in the future
	auto output = new SolverDefinitionOutput(name);
	impl->m_outputs.insert({name, output});
	return output;
}
