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

#include <misc/xmlsupport.h>

class SolverDefinitionGridType::Impl
{
public:
	/// Constructor
	Impl(const QString& name, const QString& caption, SolverDefinitionGridType* parent);
	/// Constructor
	Impl(QDomElement node, const SolverDefinitionTranslator& translator, bool isPrimary, SolverDefinitionGridType* parent);

	/// Destructor
	~Impl();

	void load(const QDomElement& node, const SolverDefinitionTranslator& translator);
	void setGridType(const QDomElement& elem);
	void setupGridRelatedConditions(const QDomNode& node, const SolverDefinitionTranslator& translator);
	void setupBoundaryConditions(const QDomNode& node, const SolverDefinitionTranslator& translator);
	void buildGridRelatedConditions(Grid* grid) const;
	Grid* createEmptyGrid();

	QString m_name;
	QString m_caption;
	bool m_isPrimary {true};
	bool m_multiple {false};
	bool m_isOptional {false};
	QList<GridType> m_availableGridTypes;
	GridType m_defaultGridType {gtUnknownGrid};
	QList<SolverDefinitionGridAttribute*> m_gridRelatedConditions;
	QMap<QString, SolverDefinitionGridAttribute*> m_gridRelatedConditionNameMap;
	QList<SolverDefinitionGridComplexAttribute*> m_gridRelatedComplexConditions;
	QMap<QString, SolverDefinitionGridComplexAttribute*> m_gridRelatedComplexConditionNameMap;
	QList<SolverDefinitionBoundaryCondition*> m_boundaryConditions;
	QMap<QString, SolverDefinitionBoundaryCondition*> m_boundaryConditionNameMap;
	QMap<QString, QString> m_solutionCaptions;

	Grid* m_emptyGrid {nullptr};
	SolverDefinitionGridType* m_parent;
};

SolverDefinitionGridType::Impl::Impl(const QString& name, const QString& caption, SolverDefinitionGridType* parent) :
	m_name {name},
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
	m_name = node.attribute("name", "default");
	m_caption = translator.translate(node.attribute("caption", QObject::tr("Default")));
	QString multistr = node.attribute("multiple", "false");
	m_multiple = (multistr == "true");
	QString optstr = node.attribute("optional", "false");
	m_isOptional = (optstr == "true");
	// set grid type
	setGridType(node);
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
	m_availableGridTypes.clear();
	if (gtype == "1d") {
		m_defaultGridType = gtNormal1DGrid;
	} else if (gtype == "1.5d") {
		m_defaultGridType = gtNormal1_5DGrid;
	} else if (gtype == "1.5d_withcrosssection") {
		m_defaultGridType = gtNormal1_5DGridWithCrosssection;
	} else if (gtype == "structured2d") {
		m_defaultGridType = gtStructured2DGrid;
	} else if (gtype == "unstructured2d") {
		m_defaultGridType = gtUnstructured2DGrid;
	} else {
		m_defaultGridType = gtUnknownGrid;
	}
	if (m_defaultGridType != gtUnknownGrid) {
		m_availableGridTypes.append(m_defaultGridType);
	}
}

void SolverDefinitionGridType::Impl::setupGridRelatedConditions(const QDomNode& node, const SolverDefinitionTranslator& translator)
{
	QDomNode itemNode = node.firstChild();
	while (! itemNode.isNull()) {
		QDomNode defNode = iRIC::getChildNode(itemNode, "Definition");
		QDomElement itemElem = itemNode.toElement();
		QDomElement defElem = defNode.toElement();

		if (defElem.attribute("valueType") == "complex") {
			// Complex condition
			SolverDefinitionGridComplexAttribute* c = new SolverDefinitionGridComplexAttribute(itemElem, translator);
			m_gridRelatedComplexConditions.append(c);
			m_gridRelatedComplexConditionNameMap.insert(c->name(), c);
		} else {
			SolverDefinitionGridAttribute* c = nullptr;
			QString pos = defElem.attribute("position");
			if (defElem.attribute("position") == "cell") {
				if (defElem.attribute("valueType") == "integer") {
					if (InputConditionWidget::hasEnums(defElem)) {
						c = new SolverDefinitionGridAttributeIntegerOptionCell(itemElem, translator);
					} else {
						c = new SolverDefinitionGridAttributeIntegerCell(itemElem, translator);
					}
				} else if (defElem.attribute("valueType") == "real") {
					if (InputConditionWidget::hasEnums(defElem)) {
						c = new SolverDefinitionGridAttributeRealOptionCell(itemElem, translator);
					} else {
						c = new SolverDefinitionGridAttributeRealCell(itemElem, translator);
					}
				}
			} else if (defElem.attribute("position") == "node") {
				if (defElem.attribute("valueType") == "integer") {
					if (InputConditionWidget::hasEnums(defElem)) {
						c = new SolverDefinitionGridAttributeIntegerOptionNode(itemElem, translator);
					} else {
						c = new SolverDefinitionGridAttributeIntegerNode(itemElem, translator);
					}
				} else if (defElem.attribute("valueType") == "real") {
					if (InputConditionWidget::hasEnums(defElem)) {
						c = new SolverDefinitionGridAttributeRealOptionNode(itemElem, translator);
					} else {
						c = new SolverDefinitionGridAttributeRealNode(itemElem, translator);
					}
				}
			}
			if (c != nullptr) {
				m_gridRelatedConditions.append(c);
				m_gridRelatedConditionNameMap.insert(c->name(), c);
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
		grid->addGridRelatedCondition(cond->container(grid));
	}
	for (SolverDefinitionGridComplexAttribute* cond : m_gridRelatedComplexConditions) {
		grid->addGridRelatedCondition(cond->container(grid));
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

SolverDefinitionGridType::SolverDefinitionGridType(const QString& name, const QString& caption) :
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

SolverDefinitionGridAttribute* SolverDefinitionGridType::gridRelatedCondition(const QString& name) const
{
	return m_impl->m_gridRelatedConditionNameMap.value(name);
}

const QList<SolverDefinitionGridComplexAttribute*>& SolverDefinitionGridType::gridRelatedComplexConditions() const
{
	return m_impl->m_gridRelatedComplexConditions;
}

SolverDefinitionGridComplexAttribute* SolverDefinitionGridType::gridRelatedComplexCondition(const QString& name) const
{
	return m_impl->m_gridRelatedComplexConditionNameMap.value(name);
}

const QList<SolverDefinitionBoundaryCondition*>& SolverDefinitionGridType::boundaryConditions() const
{
	return m_impl->m_boundaryConditions;
}

SolverDefinitionBoundaryCondition* SolverDefinitionGridType::boundaryCondition(const QString& name) const
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

const QString& SolverDefinitionGridType::name() const
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
	return m_impl->createEmptyGrid();
}

QString SolverDefinitionGridType::solutionCaption(const QString& name) const
{
	return name;
}
