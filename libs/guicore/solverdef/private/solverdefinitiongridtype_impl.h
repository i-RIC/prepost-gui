#ifndef SOLVERDEFINITIONGRIDTYPE_IMPL_H
#define SOLVERDEFINITIONGRIDTYPE_IMPL_H

#include "../solverdefinitiongridtype.h"

#include <QString>
#include <QList>
#include <QMap>

#include <string>
#include <unordered_map>

class SolverDefinitionGridType::Impl
{
public:
	Impl(const std::string& name, const QString& caption, SolverDefinitionGridType* parent);
	Impl(const QDomElement& node, SolverDefinition* solverDef, bool isPrimary, SolverDefinitionGridType* parent);

	~Impl();

	void load(const QDomElement& node, SolverDefinition* solverDef);
	void setGridType(const QDomElement& elem);
	void setGridGenerators(const QDomElement& elem);
	void setupGridAttributes(const QDomElement& elem, SolverDefinition* solverDef, const SolverDefinitionTranslator& translator);
	void setupBoundaryConditions(const QDomElement& elem, SolverDefinition* solverDef);
	void buildGridAttributes(Grid* grid) const;
	Grid* createEmptyGrid();

	std::string m_name;
	QString m_caption;
	bool m_isPrimary {true};
	bool m_multiple {false};
	bool m_isOptional {false};
	bool m_isKeepOrder {false};
	bool m_post {false};

	std::vector<GridType> m_availableGridTypes;
	std::vector<QString> m_availableGridGenerators;
	GridType m_defaultGridType {gtUnknownGrid};
	std::vector<SolverDefinitionGridAttribute*> m_gridAttributes;
	std::unordered_map<std::string, SolverDefinitionGridAttribute*> m_gridAttributeNameMap;
	std::vector<SolverDefinitionGridComplexAttribute*> m_gridComplexAttributes;
	std::unordered_map<std::string, SolverDefinitionGridComplexAttribute*> m_gridComplexAttributeNameMap;
	std::vector<SolverDefinitionBoundaryCondition*> m_boundaryConditions;
	std::unordered_map<std::string, SolverDefinitionBoundaryCondition*> m_boundaryConditionNameMap;
	std::vector<SolverDefinitionGridOutput*> m_outputs;
	std::unordered_map<std::string, SolverDefinitionGridOutput*> m_outputNameMap;

	Grid* m_emptyGrid {nullptr};
	SolverDefinitionGridType* m_parent;
};

#endif // SOLVERDEFINITIONGRIDTYPE_IMPL_H
