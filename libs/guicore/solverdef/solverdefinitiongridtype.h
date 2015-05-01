#ifndef SOLVERDEFINITIONGRIDTYPE_H
#define SOLVERDEFINITIONGRIDTYPE_H

#include "../guicore_global.h"
#include "solverdefinitionnode.h"
#include <QList>
#include <QMap>

class SolverDefinitionGridRelatedCondition;
class SolverDefinitionGridRelatedComplexCondition;
class SolverDefinitionBoundaryCondition;
class Grid;
class PreProcessorDataItem;
class PreProcessorGridDataItem;

class GUICOREDLL_EXPORT SolverDefinitionGridType : public SolverDefinitionNode
{
public:
	enum GridType {
		gtUnknownGrid,
		gtNormal1DGrid,
		gtNormal1_5DGrid,
		gtNormal1_5DGridWithCrosssection,
		gtStructured2DGrid,
		gtUnstructured2DGrid
	};
	/// constructor for dummy grid
	SolverDefinitionGridType(const QString& name, const QString& caption);
	/// constructor
	SolverDefinitionGridType(QDomElement node, const SolverDefinitionTranslator& translator, bool isPrimary)
		: SolverDefinitionNode(node, translator) {
		load(node, translator);
		m_isPrimary = isPrimary;
	}
	~SolverDefinitionGridType();
	const QList<SolverDefinitionGridRelatedCondition*>& gridRelatedConditions() const {return m_gridRelatedConditions;}
	SolverDefinitionGridRelatedCondition* gridRelatedCondition(const QString& name) const {
		return m_gridRelatedConditionNameMap.value(name);
	}
	const QList<SolverDefinitionGridRelatedComplexCondition*>& gridRelatedComplexConditions() const {return m_gridRelatedComplexConditions;}
	SolverDefinitionGridRelatedComplexCondition* gridRelatedComplexCondition(const QString& name) const {
		return m_gridRelatedComplexConditionNameMap.value(name);
	}
	const QList<SolverDefinitionBoundaryCondition*>& boundaryConditions() const {return m_boundaryConditions;}
	SolverDefinitionBoundaryCondition* boundaryCondition(const QString& name) const {
		return m_boundaryConditionNameMap.value(name);
	}
	const QList<GridType>& availableGridTypes() const {return m_availableGridTypes;}
	GridType defaultGridType() const {return m_defaultGridType;}
	const QString& name() const {return m_name;}
	const QString& caption() const {return m_caption;}
	void setCaption(const QString& caption) {m_caption = caption;}
	bool isPrimary() const {return m_isPrimary;}
	bool multiple() const {return m_multiple;}
	bool isOptional() const {return m_isOptional;}
	void buildGridRelatedConditions(Grid* grid) const;
	/// Returns a pointer to a grid that has no data.
	Grid* emptyGrid() {return m_emptyGrid;}
//	PreProcessorGridDataItem* createGridDataItem(PreProcessorDataItem* parent);
	Grid* createEmptyGrid();
	const QString solutionCaption(const QString& name);
protected:
	void load(const QDomElement& node, const SolverDefinitionTranslator& translator);
	QString m_name;
	QString m_caption;
	bool m_isPrimary;
	bool m_multiple;
	bool m_isOptional;
	QList<GridType> m_availableGridTypes;
	GridType m_defaultGridType;
	QList<SolverDefinitionGridRelatedCondition*> m_gridRelatedConditions;
	QMap<QString, SolverDefinitionGridRelatedCondition*> m_gridRelatedConditionNameMap;
	QList<SolverDefinitionGridRelatedComplexCondition*> m_gridRelatedComplexConditions;
	QMap<QString, SolverDefinitionGridRelatedComplexCondition*> m_gridRelatedComplexConditionNameMap;
	QList<SolverDefinitionBoundaryCondition*> m_boundaryConditions;
	QMap<QString, SolverDefinitionBoundaryCondition*> m_boundaryConditionNameMap;
	QMap<QString, QString> m_solutionCaptions;
private:
	void setGridType(const QDomElement& elem);
	void setupGridRelatedConditions(const QDomNode& node, const SolverDefinitionTranslator& translator);
	void setupBoundaryConditions(const QDomNode& node, const SolverDefinitionTranslator& translator);
	Grid* m_emptyGrid;
};

#endif // SOLVERDEFINITIONGRIDTYPE_H
