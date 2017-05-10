#ifndef SOLVERDEFINITIONGRIDTYPE_IMPL_H
#define SOLVERDEFINITIONGRIDTYPE_IMPL_H

#include "../solverdefinitiongridtype.h"

#include <QString>
#include <QList>
#include <QMap>

#include <string>

class SolverDefinitionGridType::Impl
{
public:
	Impl(const std::string& name, const QString& caption, SolverDefinitionGridType* parent);
	Impl(const QDomElement& node, SolverDefinition* solverDef, bool isPrimary, SolverDefinitionGridType* parent);

	~Impl();

	void load(const QDomElement& node, SolverDefinition* solverDef);
	void setGridType(const QDomElement& elem);
	void setGridGenerators(const QDomElement& elem);
	void setupGridAttributes(const QDomElement& elem, SolverDefinition* solverDef);
	void setupBoundaryConditions(const QDomElement& elem, SolverDefinition* solverDef);
	void buildGridAttributes(Grid* grid) const;
	Grid* createEmptyGrid();

	std::string m_name;
	QString m_caption;
	bool m_isPrimary {true};
	bool m_multiple {false};
	bool m_isOptional {false};
	bool m_isKeepOrder {false};
	QList<GridType> m_availableGridTypes;
	QList<QString> m_availableGridGenerators;
	GridType m_defaultGridType {gtUnknownGrid};
	QList<SolverDefinitionGridAttribute*> m_gridAttributes;
	QMap<std::string, SolverDefinitionGridAttribute*> m_gridAttributeNameMap;
	QList<SolverDefinitionGridComplexAttribute*> m_gridComplexAttributes;
	QMap<std::string, SolverDefinitionGridComplexAttribute*> m_gridComplexAttributeNameMap;
	QList<SolverDefinitionBoundaryCondition*> m_boundaryConditions;
	QMap<std::string, SolverDefinitionBoundaryCondition*> m_boundaryConditionNameMap;
	QMap<std::string, QString> m_solutionCaptions;

	Grid* m_emptyGrid {nullptr};
	SolverDefinitionGridType* m_parent;
};

#endif // SOLVERDEFINITIONGRIDTYPE_IMPL_H
