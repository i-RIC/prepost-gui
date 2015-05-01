#include "../pre/grid/grid.h"
#include "../pre/grid/structured15dgridwithcrosssection.h"
#include "../pre/grid/structured2dgrid.h"
#include "../pre/grid/unstructured2dgrid.h"
#include "../project/inputcond/cgnsfileinputconditionwidget.h"
#include "solverdefinitionboundarycondition.h"
#include "solverdefinitiongridrelatedcomplexcondition.h"
#include "solverdefinitiongridrelatedcondition.h"
#include "solverdefinitiongridtype.h"
#include "solverdefinitionintegercellgridrelatedcondition.h"
#include "solverdefinitionintegernodegridrelatedcondition.h"
#include "solverdefinitionintegeroptioncellgridrelatedcondition.h"
#include "solverdefinitionintegeroptionnodegridrelatedcondition.h"
#include "solverdefinitionrealcellgridrelatedcondition.h"
#include "solverdefinitionrealnodegridrelatedcondition.h"
#include "solverdefinitionrealoptioncellgridrelatedcondition.h"
#include "solverdefinitionrealoptionnodegridrelatedcondition.h"
//#include "pre/datamodel/preprocessornormal15dgridwithcrosssectiondataitem.h"
//#include "pre/datamodel/preprocessorstructured2dgriddataitem.h"
//#include "pre/datamodel/preprocessorunstructured2dgriddataitem.h"

#include <misc/xmlsupport.h>

SolverDefinitionGridType::SolverDefinitionGridType(const QString& name, const QString& caption)
	: SolverDefinitionNode()
{
	m_name = name;
	m_caption = caption;
	m_multiple = false;
	m_isOptional = false;
	m_isPrimary = true;
	m_defaultGridType = gtUnknownGrid;
	m_emptyGrid = nullptr;
}

void SolverDefinitionGridType::load(const QDomElement& node, const SolverDefinitionTranslator& translator)
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

SolverDefinitionGridType::~SolverDefinitionGridType()
{
	for (auto it = m_gridRelatedConditions.begin(); it != m_gridRelatedConditions.end(); ++it) {
		delete *it;
	}
	if (m_emptyGrid != nullptr) {
		delete m_emptyGrid;
	}
}

void SolverDefinitionGridType::setGridType(const QDomElement& elem)
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

void SolverDefinitionGridType::setupGridRelatedConditions(const QDomNode& node, const SolverDefinitionTranslator& translator)
{
	QDomNode itemNode = node.firstChild();
	while (! itemNode.isNull()) {
		QDomNode defNode = iRIC::getChildNode(itemNode, "Definition");
		QDomElement itemElem = itemNode.toElement();
		QDomElement defElem = defNode.toElement();

		if (defElem.attribute("valueType") == "complex") {
			// Complex condition
			SolverDefinitionGridRelatedComplexCondition* c = new SolverDefinitionGridRelatedComplexCondition(itemElem, translator);
			m_gridRelatedComplexConditions.append(c);
			m_gridRelatedComplexConditionNameMap.insert(c->name(), c);
		} else {
			SolverDefinitionGridRelatedCondition* c = nullptr;
			QString pos = defElem.attribute("position");
			if (defElem.attribute("position") == "cell") {
				if (defElem.attribute("valueType") == "integer") {
					if (CgnsFileInputConditionWidget::hasEnums(defElem)) {
						c = new SolverDefinitionIntegerOptionCellGridRelatedCondition(itemElem, translator);
					} else {
						c = new SolverDefinitionIntegerCellGridRelatedCondition(itemElem, translator);
					}
				} else if (defElem.attribute("valueType") == "real") {
					if (CgnsFileInputConditionWidget::hasEnums(defElem)) {
						c = new SolverDefinitionRealOptionCellGridRelatedCondition(itemElem, translator);
					} else {
						c = new SolverDefinitionRealCellGridRelatedCondition(itemElem, translator);
					}
				}
			} else if (defElem.attribute("position") == "node") {
				if (defElem.attribute("valueType") == "integer") {
					if (CgnsFileInputConditionWidget::hasEnums(defElem)) {
						c = new SolverDefinitionIntegerOptionNodeGridRelatedCondition(itemElem, translator);
					} else {
						c = new SolverDefinitionIntegerNodeGridRelatedCondition(itemElem, translator);
					}
				} else if (defElem.attribute("valueType") == "real") {
					if (CgnsFileInputConditionWidget::hasEnums(defElem)) {
						c = new SolverDefinitionRealOptionNodeGridRelatedCondition(itemElem, translator);
					} else {
						c = new SolverDefinitionRealNodeGridRelatedCondition(itemElem, translator);
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

void SolverDefinitionGridType::setupBoundaryConditions(const QDomNode& node, const SolverDefinitionTranslator& translator)
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


void SolverDefinitionGridType::buildGridRelatedConditions(Grid* grid) const
{
	for (auto it = m_gridRelatedConditions.begin(); it != m_gridRelatedConditions.end(); ++it) {
		SolverDefinitionGridRelatedCondition* cond = *it;
		grid->addGridRelatedCondition(cond->container(grid));
	}
	for (auto cit = m_gridRelatedComplexConditions.begin(); cit != m_gridRelatedComplexConditions.end(); ++cit) {
		SolverDefinitionGridRelatedComplexCondition* cond = *cit;
		grid->addGridRelatedCondition(cond->container(grid));
	}
}

Grid* SolverDefinitionGridType::createEmptyGrid()
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

const QString SolverDefinitionGridType::solutionCaption(const QString& name)
{
	if (m_solutionCaptions.contains(name)) {
		return m_solutionCaptions.value(name);
	} else {
		return name;
	}
}

/*
PreProcessorGridDataItem* SolverDefinitionGridType::createGridDataItem(PreProcessorDataItem* parent)
{
	switch (m_defaultGridType)
	{
	case gtNormal1DGrid:
		// @todo not implemented yet.
		return nullptr;
		break;
	case gtNormal1_5DGrid:
		// @todo not implemented yet.
		return nullptr;
		break;
	case gtNormal1_5DGridWithCrosssection:
		return new PreProcessorNormal15DGridWithCrossSectionDataItem(parent);
		break;
	case gtStructured2DGrid:
		return new PreProcessorStructured2dGridDataItem(parent);
		break;
	case gtUnstructured2DGrid:
		return new PreProcessorUnstructured2dGridDataItem(parent);
		break;
	case gtUnknownGrid:
	default:
		return nullptr;
		break;
	}
}
*/
