#include "../pre/grid/grid.h"
#include "../pre/grid/structured15dgridwithcrosssection.h"
#include "../pre/grid/structured2dgrid.h"
#include "../pre/grid/unstructured2dgrid.h"
#include "../project/inputcond/inputconditionwidget.h"
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

SolverDefinitionGridType::Impl::Impl(QDomElement node, const SolverDefinitionTranslator& translator, bool isPrimary, SolverDefinitionGridType* parent) :
	m_isPrimary {isPrimary},
	m_parent {parent}
{
	load(node, translator);
}

SolverDefinitionGridType::Impl::~Impl()
{
	for (auto c : m_gridRelatedConditions) {
		delete c;
	}
	delete m_emptyGrid;
}

void SolverDefinitionGridType::Impl::load(const QDomElement& node, const SolverDefinitionTranslator& translator)
{
	// set basic informations.
	m_name = iRIC::toStr(node.attribute("name", "default"));
	m_caption = translator.translate(node.attribute("caption", QObject::tr("Default")));
	QString multistr = node.attribute("multiple", "false");
	m_multiple = (multistr == "true");
	QString optstr = node.attribute("optional", "false");
	m_isOptional = (optstr == "true");
	// set grid type
	setGridType(node);
	// set grid generators
	setGridGenerators(node);
	// setup grid related conditions
	QDomNode grcNode = iRIC::getChildNode(node, "GridRelatedCondition");
	setupGridRelatedConditions(grcNode, translator);
	// setup boundary conditions;
	setupBoundaryConditions(node, translator);
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

void SolverDefinitionGridType::Impl::setupGridRelatedConditions(const QDomNode& node, const SolverDefinitionTranslator& translator)
{
	QDomElement elem = node.toElement();
	m_isKeepOrder = (elem.attribute("keepOrder") == "true");
	QDomNode itemNode = node.firstChild();
	int order = 1;
	while (! itemNode.isNull()) {
		QDomNode defNode = iRIC::getChildNode(itemNode, "Definition");
		QDomElement itemElem = itemNode.toElement();
		QDomElement defElem = defNode.toElement();

		if (defElem.attribute("valueType") == "complex") {
			// Complex condition
			SolverDefinitionGridComplexAttribute* c = new SolverDefinitionGridComplexAttribute(itemElem, translator, order);
			m_gridRelatedComplexConditions.append(c);
			m_gridRelatedComplexConditionNameMap.insert(c->name(), c);
		} else {
			SolverDefinitionGridAttribute* c = nullptr;
			QString pos = defElem.attribute("position");
			if (defElem.attribute("position") == "cell") {
				if (defElem.attribute("valueType") == "integer") {
					if (InputConditionWidget::hasEnums(defElem)) {
						c = new SolverDefinitionGridAttributeIntegerOptionCell(itemElem, translator, order);
					} else {
						c = new SolverDefinitionGridAttributeIntegerCell(itemElem, translator, order);
					}
				} else if (defElem.attribute("valueType") == "real") {
					if (InputConditionWidget::hasEnums(defElem)) {
						c = new SolverDefinitionGridAttributeRealOptionCell(itemElem, translator, order);
					} else {
						c = new SolverDefinitionGridAttributeRealCell(itemElem, translator, order);
					}
				}
			} else if (defElem.attribute("position") == "node") {
				if (defElem.attribute("valueType") == "integer") {
					if (InputConditionWidget::hasEnums(defElem)) {
						c = new SolverDefinitionGridAttributeIntegerOptionNode(itemElem, translator, order);
					} else {
						c = new SolverDefinitionGridAttributeIntegerNode(itemElem, translator, order);
					}
				} else if (defElem.attribute("valueType") == "real") {
					if (InputConditionWidget::hasEnums(defElem)) {
						c = new SolverDefinitionGridAttributeRealOptionNode(itemElem, translator, order);
					} else {
						c = new SolverDefinitionGridAttributeRealNode(itemElem, translator, order);
					}
				}
			}
			if (c != nullptr) {
				m_gridRelatedConditions.append(c);
				m_gridRelatedConditionNameMap.insert(c->name(), c);
				++ order;
			}
		}
		itemNode = itemNode.nextSibling();
	}
}

void SolverDefinitionGridType::Impl::setupBoundaryConditions(const QDomNode& node, const SolverDefinitionTranslator& translator)
{
	QDomNode itemNode = node.firstChild();
	while (! itemNode.isNull()) {
		if (itemNode.nodeName() != "BoundaryCondition") {
			itemNode = itemNode.nextSibling();
			continue;
		}
		SolverDefinitionBoundaryCondition* b = new SolverDefinitionBoundaryCondition(itemNode.toElement(), translator);
		m_boundaryConditions.append(b);
		m_boundaryConditionNameMap.insert(b->name(), b);
		itemNode = itemNode.nextSibling();
	}
}

void SolverDefinitionGridType::Impl::buildGridRelatedConditions(Grid* grid) const
{
	for (SolverDefinitionGridAttribute* cond : m_gridRelatedConditions) {
		grid->addGridAttribute(cond->container(grid));
	}
	for (SolverDefinitionGridComplexAttribute* cond : m_gridRelatedComplexConditions) {
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
		buildGridRelatedConditions(ret);
	}
	return ret;
}

// Public interfaces

SolverDefinitionGridType::SolverDefinitionGridType(const std::string& name, const QString& caption) :
	SolverDefinitionNode {},
	m_impl {new Impl{name, caption, this}}
{}

SolverDefinitionGridType::SolverDefinitionGridType(QDomElement node, const SolverDefinitionTranslator& translator, bool isPrimary) :
	SolverDefinitionNode {node,translator},
	m_impl {new Impl {node, translator, isPrimary, this}}
{}

SolverDefinitionGridType::~SolverDefinitionGridType()
{
	delete m_impl;
}

const QList<SolverDefinitionGridAttribute*>& SolverDefinitionGridType::gridRelatedConditions() const
{
	return m_impl->m_gridRelatedConditions;
}

SolverDefinitionGridAttribute* SolverDefinitionGridType::gridRelatedCondition(const std::string& name) const
{
	return m_impl->m_gridRelatedConditionNameMap.value(name);
}

const QList<SolverDefinitionGridComplexAttribute*>& SolverDefinitionGridType::gridRelatedComplexConditions() const
{
	return m_impl->m_gridRelatedComplexConditions;
}

SolverDefinitionGridComplexAttribute* SolverDefinitionGridType::gridRelatedComplexCondition(const std::string& name) const
{
	return m_impl->m_gridRelatedComplexConditionNameMap.value(name);
}

const QList<SolverDefinitionBoundaryCondition*>& SolverDefinitionGridType::boundaryConditions() const
{
	return m_impl->m_boundaryConditions;
}

SolverDefinitionBoundaryCondition* SolverDefinitionGridType::boundaryCondition(const std::string& name) const
{
	return m_impl->m_boundaryConditionNameMap.value(name);
}

const QList<SolverDefinitionGridType::GridType>& SolverDefinitionGridType::availableGridTypes() const
{
	return m_impl->m_availableGridTypes;
}

SolverDefinitionGridType::GridType SolverDefinitionGridType::defaultGridType() const
{
	return m_impl->m_defaultGridType;
}

const QList<QString>& SolverDefinitionGridType::availableGridGenerators() const
{
	return m_impl->m_availableGridGenerators;
}

const std::string& SolverDefinitionGridType::name() const
{
	return m_impl->m_name;
}

const QString& SolverDefinitionGridType::caption() const
{
	return m_impl->m_caption;
}

void SolverDefinitionGridType::setCaption(const QString& caption)
{
	m_impl->m_caption = caption;
}

bool SolverDefinitionGridType::isPrimary() const
{
	return m_impl->m_isPrimary;
}

bool SolverDefinitionGridType::multiple() const
{
	return m_impl->m_multiple;
}

bool SolverDefinitionGridType::isOptional() const
{
	return m_impl->m_isOptional;
}

bool SolverDefinitionGridType::isKeepOrder() const
{
	return m_impl->m_isKeepOrder;
}

void SolverDefinitionGridType::buildGridRelatedConditions(Grid* grid) const
{
	m_impl->buildGridRelatedConditions(grid);
}

Grid* SolverDefinitionGridType::emptyGrid() const
{
	return m_impl->m_emptyGrid;
}

Grid* SolverDefinitionGridType::createEmptyGrid()
{
	return createEmptyGrid(m_impl->m_defaultGridType);
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
		buildGridRelatedConditions(ret);
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
