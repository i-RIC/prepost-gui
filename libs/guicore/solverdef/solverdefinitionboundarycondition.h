#ifndef SOLVERDEFINITIONBOUNDARYCONDITION_H
#define SOLVERDEFINITIONBOUNDARYCONDITION_H

#include "../guicore_global.h"
#include "solverdefinitionnode.h"
#include <QString>
#include <QDomElement>

class SolverDefinition;

class GUICOREDLL_EXPORT SolverDefinitionBoundaryCondition : public SolverDefinitionNode
{
public:
	enum Position {pNode, pCell, pEdge};
	SolverDefinitionBoundaryCondition(QDomElement node, const SolverDefinitionTranslator& translator)
		: SolverDefinitionNode(node, translator)
	{
		load(node, translator);
	}
	~SolverDefinitionBoundaryCondition(){}
	const QString& name() const {return m_name;}
	const QString& caption() const {return m_caption;}
	const QString& englishCaption() const {return m_englishCaption;}
	void setCaption(const QString& caption){m_caption = caption;}
	const QDomElement& element(){return m_element;}
	Position position(){return m_position;}
//	PreProcessorBCDataItem* createConditionDataItem(SolverDefinition* def, PreProcessorDataItem* parent);

private:
	void load(const QDomElement& node, const SolverDefinitionTranslator& translator);
	QString m_name;
	QString m_caption;
	QString m_englishCaption;
	Position m_position;
	QDomElement m_element;
	SolverDefinition* m_definition;
};

#endif // SOLVERDEFINITIONBOUNDARYCONDITION_H
